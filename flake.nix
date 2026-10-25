{
  description = "FallingSand SDL3 application";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
    let
      systems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forAllSystems = function:
        nixpkgs.lib.genAttrs systems (system:
          function (import nixpkgs { inherit system; }));
    in
    {
      packages = forAllSystems (pkgs: {
        default = pkgs.stdenv.mkDerivation {
          pname = "fallingsand";
          version = "1.0.0";
          src = self;

          nativeBuildInputs = [ pkgs.cmake pkgs.pkg-config pkgs.makeWrapper ];
          buildInputs = [ pkgs.sdl3 pkgs.sdl3-ttf pkgs.sdl3-image];

          cmakeBuildType = "Release";

          installPhase = ''
            runHook preInstall
            mkdir -p "$out/bin" "$out/libexec" "$out/share/fallingsand"
            cp bin/FallingSand "$out/libexec/FallingSand"
            cp -R "$src/assets" "$out/share/fallingsand/"
            makeWrapper "$out/libexec/FallingSand" "$out/bin/FallingSand" \
              --run "cd '$out/share/fallingsand'"
            runHook postInstall
          '';

          meta = {
            description = "Falling sand simulator built with SDL3";
            mainProgram = "FallingSand";
            platforms = systems;
          };
        };
      });

      devShells = forAllSystems (pkgs: {
        default = pkgs.mkShell {
          packages = [ pkgs.cmake pkgs.pkg-config pkgs.sdl3 pkgs.sdl3-ttf pkgs.sdl3-image];
        };
      });
    };
}
