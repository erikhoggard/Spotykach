{
  description = "Spotykach firmware dev environment (Daisy Seed / STM32H7)";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = f: nixpkgs.lib.genAttrs systems (system: f nixpkgs.legacyPackages.${system});
    in
    {
      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = with pkgs; [
            # arm-none-eabi-gcc/g++ + newlib. Pinned to 13.x on purpose: the default
            # gcc-arm-embedded (15.2) generates code that overflows the tight 186 KB
            # SRAM_EXEC region of the BOOT_SRAM layout (100.58%). Daisy targets the
            # 10.3 toolchain; 13.3 is the oldest packaged here and fits (98.59%).
            gcc-arm-embedded-13
            gnumake  # `make -j8` etc.
            dfu-util # `make program-dfu` flashing over USB DFU
            git      # submodule fetch (libDaisy + DaisySP)
          ];

          shellHook = ''
            echo "Spotykach dev shell: $(arm-none-eabi-gcc -dumpversion 2>/dev/null) | make $(make -v 2>/dev/null | head -n1 | cut -d' ' -f3) | dfu-util ready"
            if [ ! -e lib/libDaisy/core/Makefile ]; then
              echo "NOTE: submodules not checked out yet -> run: git submodule update --init --recursive"
            fi
          '';
        };
      });
    };
}
