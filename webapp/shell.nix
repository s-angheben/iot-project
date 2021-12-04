with import <nixpkgs> {};

stdenv.mkDerivation {
	name = "node";
	buildInputs = [
	  nodejs
	  mosquitto
	];
	shellHook = ''
	  export PATH="$PWD/node_modules/.bin/:$PATH"
	'';
}
