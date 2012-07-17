uhr
===

[Sun Wah Linux Unihan 2]{http://www.sw-linux.com/en/scripts/main/viewitem.php?itemid=2158511108981584&showid=products&catid=platforms} is a Hanzi input platfrom from [Sun Wah]{http://www.sw-linux.com/en/scripts/main/index.php}

uhr is a very simple wrapper that exposes the handwriting recogniser as a standalone Linux stdin/stdout binary

Protocol
--------
uhr will listen on stdin for binary data. The first 16 bits are interpreted as the length of the following message. The remaining bits are coordinate pairs (x,y), repeating to describe a stroke of a chinese character. Strokes are separated by (0xff,0xff).

After reading a message from stdin, uhr will output the 9 most likely candidates in UTF-8 to stdout, once again preceeded by a 16-bit integer describing the length of the following UTF-8 string

uhr will then listen on stdin again, until it receives an EOF
