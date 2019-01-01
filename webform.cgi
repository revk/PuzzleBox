#!/bin/csh -f
cat << END
Content-Type: text/html

<html>
<head>
<style>
body {
	background-color:#224;
	background-image: linear-gradient(to bottom right,#224, #448);
	color:white;
	font-family:Raleway,sans-serif;
	margin:4ex;
}
img{
	border-radius: 13px;
	box-shadow: 5px 5px 5px black;
	border:5px;
}
a {color: white;}
</style>
</head>
<body>
	<h1><img src='puzzlebox.jpg' style='float:right'>Puzzle Box</h1>
<p>This makes a customised puzzle box with random maze. Use <a href='https://www.openscad.org'>OpenSCAD</a> to render to an STL for 3D printing.</p>
<p>See <a href='https://www.thingiverse.com/thing:2410748'>https://www.thingiverse.com/thing:2410748</a> for examples.</p>
<p>Do click <i>like</i> on thingiverse. Gifts <a href='https://monzo.me/adriankennard'>accepted</a> (just because people have asked how they can do this).</p>
<form method=get action='puzzlebox.cgi'>
	<input type=submit value='Get OpenSCAD file'>
	<table>
END
./puzzlebox.cgi --web-form
cat << END
	</table>
	<input type=submit value='Get OpenSCAD file'>
</form>
<p><i>Some parameters are subject to sanity checks, e.g. nubs set to helix if greater than 1, sides is multiple of nubs, etc. See comments at top of output for parameters actually used.</i></p>
<p>Source code at <a href='https://github.com/revk/PuzzleBox'>https://github.com/revk/PuzzleBox</a></p>
<hr/>
<address>By RevK<sup>&reg;</sup> <a href='https://www.me.uk/'>www.me.uk</a> <a href='https://twitter.com/TheRealRevK'>@TheRealRevK</a></address>
</body>
</html>
END
