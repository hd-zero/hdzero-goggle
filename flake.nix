# This file is the entry point used by the nix package manager.
# It declares package builds and development environments.
# See https://wiki.nixos.org/wiki/Flakes for more information on the `flake.nix` format.
# In case of questions, feel free to ask @DavHau
{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    nix-filter.url = "github:numtide/nix-filter";
  };
  outputs = {
    self,
    nixpkgs,
    nix-filter,
  }: let
    # Currently can only support x86_64-linux builders, due to the hardcoded toolchain
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};

    # Source code without the nix directory and flake files to improve build caching
    hdzero-goggle-src = nix-filter.lib {
      root = ./.;
      exclude = [ "nix" "flake.nix" "flake.lock" ];
    };
  in
  {
    # Packages that can be built via `nix build .#<name>`
    packages.${system} = {

      # the goggle app including the jffs2 image with service binaries
      goggle-app = pkgs.callPackage ./nix/goggle-app.nix {
        inherit hdzero-goggle-src;
        inherit (self.packages.${system}) toolchain;
      };

      # make the goggle app the default package
      default = self.packages.${system}.goggle-app;

      # patched toolchain to make compatible with nix build sandbox
      toolchain = pkgs.callPackage ./nix/toolchain.nix {};
    };

    # a dev environment which can be entered via `nix develop .`
    devShells.${system}.default = pkgs.callPackage ./nix/devShell.nix {
      inherit (self.packages.${system}) toolchain;
    };
  };
}
