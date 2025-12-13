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
<p>This makes a customised puzzle box with random maze.</p>
<p>See <a href='https://www.thingiverse.com/thing:2410748'>https://www.thingiverse.com/thing:2410748</a> for examples.</p>
<p>Do click <i>like</i> on thingiverse. Gifts <a href='https://monzo.me/adriankennard'>accepted</a> (just because people have asked how they can do this).</p>
<form method=get action='puzzlebox.cgi'>
	<input type=submit value='Get file'>
	<table>
END
./puzzlebox.cgi --web-form
cat << END
	</table>
	<input type=submit value='Get file'>
</form>
<p><i>Some parameters are subject to sanity checks, e.g. nubs set to helix if greater than 1, etc. See comments at top of output for parameters actually used.</i></p>
<p>Source code at <a href='https://github.com/revk/PuzzleBox'>https://github.com/revk/PuzzleBox</a></p>
<p style='font-size:50%'>[Note, licence is creative commons ATTRIBUTION. A commercial licence is available if not using CCA, at £10 per print sold. If you are not attributing you are either breaking the law by using without a licence and so costing me the £10 I would get, or are agreeing to the commercial licence. Either way you owe me £10 a print, plus legal costs if I choose to sue you to get it. So ATTRIBUTE any usage of this model.]</p>
<hr/>
<address>By RevK<sup>&reg;</sup> <a href='https://www.me.uk/'>www.me.uk</a> <a href='https://twitter.com/TheRealRevK'>@TheRealRevK</a></address>
</body>
</html>
END
