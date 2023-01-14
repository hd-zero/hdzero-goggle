/**
 * \file progressbar.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2001/12/29
 * 
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
    Copyright (C) 1998~2002, WEI Yongming

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: progressbar.h 10690 2008-08-18 09:32:47Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef _MGUI_CTRL_PROGRESSBAR_H
#define _MGUI_CTRL_PROGRESSBAR_H
 

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_progbar ProgressBar control
     * @{
     */

/**
 * \def CTRL_PROGRESSBAR
 * \brief The class name of progressbar control.
 */
#define CTRL_PROGRESSBAR        ("progressbar")
/** Progressbar return value */
#define PB_OKAY                 0
/** Progressbar return value */
#define PB_ERR                  -1

    /**
     * \defgroup ctrl_progbar_styles Styles of progressbar control
     * @{
     */

/**
 * \def PBS_NOTIFY
 * \brief Notifies the parent window.
 * 
 * Sends the parent window notification messages when 
 * the user clicks or double-clicks the control.
 */
#define PBS_NOTIFY              0x0001L

/**
 * \def PBS_VERTICAL
 * \brief Creates progressbar vertically.
 */
#define PBS_VERTICAL            0x0002L

    /** @} end of ctrl_progbar_styles */

    /**
     * \defgroup ctrl_progbar_msgs Messages of progressbar control
     * @{
     */

/**
 * \def PBM_SETRANGE
 * \brief Sets the limits of the range.
 *
 * Sets the upper and lower limits of the progress bar control's range, 
 * and redraws the bar to reflect the new ranges.
 *
 * \code
 * PBM_SETRANGE
 * int min, max;
 *
 * wParam = (WPARAM)min;
 * lParam = (LPARAM)max;
 * \endcode
 *
 * \param min The lower limit of the progress bar.
 * \param max The upper limit of the progress bar.
 *
 * \return PB_OKAY on success, else PB_ERR.
 */
#define PBM_SETRANGE            0xF0A0

/**
 * \def PBM_SETSTEP
 * \brief Specifies the step increment for a progress bar control.
 *
 * \code
 * PBM_SETSTEP
 * int stepinc;
 *
 * wParam = (WPARAM)stepinc;
 * lParam = 0;
 * \endcode
 *
 * \param stepinc Step increment for a progress bar control.
 * \return PB_OKAY on success, else PB_ERR.
 */
#define PBM_SETSTEP             0xF0A1

/**
 * \def PBM_SETPOS
 * \brief Sets the progress bar control's current position.
 *
 * Sets the progress bar control's current position as specified by nPos, 
 * and redraw the bar to reflect the new position.
 *
 * \code
 * PBM_SETPOS
 * int nPos;
 *
 * wParam = (WPARAM)nPos;
 * lParam = 0;
 * \endcode
 *
 * \param nPos The progress bar control's current position.
 * \return Always be PB_OKAY.
 */
#define PBM_SETPOS              0xF0A2

/**
 * \def PBM_DELTAPOS
 * \brief Advances the progress bar control's current position. 
 *
 * Advances the progress bar control's current position as specified by posInc, 
 * and redraw the bar to reflect the new position.
 *
 * \code
 * PBM_DELTAPOS
 * int posInc;
 *
 * wParam = (WPARAM)posInc;
 * lParam = 0;
 * \endcode
 *
 * \param posInc The progress bar control's position increment.
 * \return Always be PB_OKAY.
 */
#define PBM_DELTAPOS            0xF0A3

/**
 * \def PBM_STEPIT
 * \brief Advances the current position by the step increment.
 *
 * Advances the current position for a progress bar control by 
 * the step increment, and redraw the bar to reflect the new position.
 *
 * \code
 * PBM_STEPIT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Always be PB_OKAY.
 */
#define PBM_STEPIT              0xF0A4

#define PBM_MSGMAX              0xF0B0

    /** @} end of ctrl_progbar_msgs */

    /**
     * \defgroup ctrl_progbar_ncs Notification codes of progressbar control
     * @{
     */

/**
 * \def PBN_REACHMAX
 * \brief Notifies reach of maximum limit.
 *
 * The PBN_REACHMAX notification code is sent when the progressbar reachs its maximum limit.
 */
#define PBN_REACHMAX            1

/**
 * \def PBN_REACHMIN
 * \brief Notifies reach of minimum limit.
 *
 * The PBN_REACHMIN notification code is sent when the progressbar reachs its minimum limit.
 */
#define PBN_REACHMIN            2

    /** @} end of ctrl_progbar_ncs */

    /** @} end of ctrl_progbar */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_PROGRESSBAR_H */

