// ==========================================================================
// vanc_timecode_inserter.v
//
// SMPTE ST 291M VANC Timecode Inserter for Atomos Ninja V Trigger
//
// Inserts SMPTE 12M-2 timecode as a VANC ancillary data packet into
// the vertical blanking interval of the video stream.  The Ninja V
// reads this timecode to trigger recording start/stop.
//
// Interface:
//   - 8-bit parallel video in/out with DE/HSYNC/VSYNC (before IT66121)
//   - CPU-writable timecode registers (directly mapped or via SPI/I2C)
//
// NOTE: VANC_H_START denotes the pixel cycle *before* ADF[0] appears
// on vid_data_out, due to the 1-cycle output register pipeline.
// Set VANC_H_START = (desired_ADF_pixel - 1).
//
// IMPORTANT: This is SAMPLE code — adapt to the actual FPGA video
// pipeline, clock domain, and register interface.
// ==========================================================================

module vanc_timecode_inserter #(
    parameter VANC_LINE     = 14,      // Vertical blanking line for VANC insertion
    parameter VANC_H_START  = 15       // Pixel cycle before ADF[0] output (see note above)
)(
    input  wire        clk,            // Pixel clock
    input  wire        rst_n,          // Active-low reset

    // Video input — 8-bit DE-based parallel interface (no embedded EAV/SAV)
    input  wire [7:0]  vid_data_in,
    input  wire        vid_hsync_in,
    input  wire        vid_vsync_in,
    input  wire        vid_de_in,

    // Video output (to IT66121)
    output reg  [7:0]  vid_data_out,
    output reg         vid_hsync_out,
    output reg         vid_vsync_out,
    output reg         vid_de_out,

    // Timecode registers (directly mapped from CPU or via SPI/I2C bridge)
    input  wire        tc_enable,      // 1=insert VANC timecode, 0=passthrough
    input  wire [3:0]  tc_frames_u,    // Frames units  (0-9)
    input  wire [1:0]  tc_frames_t,    // Frames tens   (0-2)
    input  wire [3:0]  tc_seconds_u,   // Seconds units (0-9)
    input  wire [2:0]  tc_seconds_t,   // Seconds tens  (0-5)
    input  wire [3:0]  tc_minutes_u,   // Minutes units (0-9)
    input  wire [2:0]  tc_minutes_t,   // Minutes tens  (0-5)
    input  wire [3:0]  tc_hours_u,     // Hours units   (0-9)
    input  wire [1:0]  tc_hours_t      // Hours tens    (0-2)
);

// --------------------------------------------------------------------------
// Line and pixel counters
// --------------------------------------------------------------------------
reg [11:0] h_count;     // Horizontal pixel counter
reg [11:0] v_count;     // Vertical line counter
reg        prev_hsync;
reg        prev_vsync;

wire h_sync_rise = vid_hsync_in & ~prev_hsync;
wire v_sync_rise = vid_vsync_in & ~prev_vsync;

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        h_count    <= 12'd0;
        v_count    <= 12'd0;
        prev_hsync <= 1'b0;
        prev_vsync <= 1'b0;
    end else begin
        prev_hsync <= vid_hsync_in;
        prev_vsync <= vid_vsync_in;

        if (v_sync_rise) begin
            v_count <= 12'd0;
            h_count <= 12'd0;   // Reset h_count on frame start for deterministic alignment
        end else if (h_sync_rise) begin
            v_count <= v_count + 12'd1;
            h_count <= 12'd0;
        end else begin
            h_count <= h_count + 12'd1;
        end
    end
end

// --------------------------------------------------------------------------
// Timecode latch — snapshot inputs at insertion start to prevent
// mid-packet changes if the CPU updates timecode during emission.
// --------------------------------------------------------------------------
reg [3:0] lat_frames_u;
reg [1:0] lat_frames_t;
reg [3:0] lat_seconds_u;
reg [2:0] lat_seconds_t;
reg [3:0] lat_minutes_u;
reg [2:0] lat_minutes_t;
reg [3:0] lat_hours_u;
reg [1:0] lat_hours_t;

// --------------------------------------------------------------------------
// VANC packet ROM — SMPTE ST 291M with SMPTE 12M-2 timecode
//
// Packet structure (8-bit mode):
//   [0]  ADF0 = 0x00
//   [1]  ADF1 = 0xFF
//   [2]  ADF2 = 0xFF
//   [3]  DID  = 0x60  (SMPTE 12M-2)
//   [4]  SDID = 0x60  (SMPTE 12M-2)
//   [5]  DC   = 0x10  (16 data words)
//   [6]  UDW0 = frames units
//   [7]  UDW1 = frames tens + flags
//   [8]  UDW2 = seconds units
//   [9]  UDW3 = seconds tens + flags
//   [10] UDW4 = minutes units
//   [11] UDW5 = minutes tens + flags
//   [12] UDW6 = hours units
//   [13] UDW7 = hours tens + flags
//   [14..21] UDW8-15 = binary groups (zeros)
//   [22] CS   = checksum (8-bit sum of DID through last UDW)
// --------------------------------------------------------------------------

localparam PKT_LEN = 23;  // Total packet words (3 ADF + DID + SDID + DC + 16 UDW + CS)

reg [7:0] pkt_rom [0:PKT_LEN-1];
reg [7:0] checksum;  // 8-bit sum (SMPTE ST 291M section 6 for 8-bit interfaces)

// Build packet combinationally from latched timecode registers
always @(*) begin
    // ADF
    pkt_rom[0]  = 8'h00;
    pkt_rom[1]  = 8'hFF;
    pkt_rom[2]  = 8'hFF;
    // DID, SDID, DC
    pkt_rom[3]  = 8'h60;
    pkt_rom[4]  = 8'h60;
    pkt_rom[5]  = 8'h10;
    // UDW0-7: timecode (BCD nibbles) — from latched values
    pkt_rom[6]  = {4'h0, lat_frames_u};
    pkt_rom[7]  = {4'h0, 2'b00, lat_frames_t};
    pkt_rom[8]  = {4'h0, lat_seconds_u};
    pkt_rom[9]  = {4'h0, 1'b0, lat_seconds_t};
    pkt_rom[10] = {4'h0, lat_minutes_u};
    pkt_rom[11] = {4'h0, 1'b0, lat_minutes_t};
    pkt_rom[12] = {4'h0, lat_hours_u};
    pkt_rom[13] = {4'h0, 2'b00, lat_hours_t};
    // UDW8-15: binary groups (zeros)
    pkt_rom[14] = 8'h00;
    pkt_rom[15] = 8'h00;
    pkt_rom[16] = 8'h00;
    pkt_rom[17] = 8'h00;
    pkt_rom[18] = 8'h00;
    pkt_rom[19] = 8'h00;
    pkt_rom[20] = 8'h00;
    pkt_rom[21] = 8'h00;
    // Checksum: 8-bit sum of DID through last UDW (SMPTE ST 291M section 6, 8-bit interface)
    checksum = pkt_rom[3]  + pkt_rom[4]  + pkt_rom[5]
             + pkt_rom[6]  + pkt_rom[7]  + pkt_rom[8]
             + pkt_rom[9]  + pkt_rom[10] + pkt_rom[11]
             + pkt_rom[12] + pkt_rom[13] + pkt_rom[14]
             + pkt_rom[15] + pkt_rom[16] + pkt_rom[17]
             + pkt_rom[18] + pkt_rom[19] + pkt_rom[20]
             + pkt_rom[21];
    pkt_rom[22] = checksum;
end

// --------------------------------------------------------------------------
// VANC insertion state machine
//
// Latches timecode at insertion start.  Aborts if vid_de_in asserts
// mid-packet (blanking shorter than expected) to prevent corrupting
// active video pixels.
// --------------------------------------------------------------------------
reg        inserting;       // 1 while overwriting pixels with VANC data
reg [4:0]  pkt_idx;         // Current byte index into pkt_rom

wire on_vanc_line = (v_count == VANC_LINE);
wire at_h_start   = (h_count == VANC_H_START);

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        inserting     <= 1'b0;
        pkt_idx       <= 5'd0;
        lat_frames_u  <= 4'd0;
        lat_frames_t  <= 2'd0;
        lat_seconds_u <= 4'd0;
        lat_seconds_t <= 3'd0;
        lat_minutes_u <= 4'd0;
        lat_minutes_t <= 3'd0;
        lat_hours_u   <= 4'd0;
        lat_hours_t   <= 2'd0;
    end else begin
        if (tc_enable && on_vanc_line && at_h_start && !vid_de_in && !inserting) begin
            // Latch timecode at insertion start — consistent packet contents
            lat_frames_u  <= tc_frames_u;
            lat_frames_t  <= tc_frames_t;
            lat_seconds_u <= tc_seconds_u;
            lat_seconds_t <= tc_seconds_t;
            lat_minutes_u <= tc_minutes_u;
            lat_minutes_t <= tc_minutes_t;
            lat_hours_u   <= tc_hours_u;
            lat_hours_t   <= tc_hours_t;
            // Start VANC insertion
            inserting <= 1'b1;
            pkt_idx   <= 5'd0;
        end else if (inserting) begin
            if (vid_de_in) begin
                // Active video started — abort insertion to avoid pixel corruption
                inserting <= 1'b0;
                pkt_idx   <= 5'd0;
            end else if (pkt_idx == PKT_LEN - 1) begin
                inserting <= 1'b0;
                pkt_idx   <= 5'd0;
            end else begin
                pkt_idx <= pkt_idx + 5'd1;
            end
        end
    end
end

// --------------------------------------------------------------------------
// Video output mux — insert VANC or pass through
// --------------------------------------------------------------------------
always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        vid_data_out  <= 8'd0;
        vid_hsync_out <= 1'b0;
        vid_vsync_out <= 1'b0;
        vid_de_out    <= 1'b0;
    end else begin
        vid_hsync_out <= vid_hsync_in;
        vid_vsync_out <= vid_vsync_in;
        vid_de_out    <= vid_de_in;

        if (inserting) begin
            vid_data_out <= pkt_rom[pkt_idx];
        end else begin
            vid_data_out <= vid_data_in;
        end
    end
end

endmodule
