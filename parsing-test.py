#!/usr/bin/env python

import pexpect

EOF = ''

# Whitespace compressing 
c = pexpect.spawn('./qweparsetest', timeout=1)
c.sendline('<top>	<foo> TEXT</foo>But  not  here</top>')
c.expect('<top><foo>TEXT</foo>But  not  here</top>')
c.send(EOF)

# Incremental parsing
c = pexpect.spawn('./qweparsetest', timeout=1)
c.sendline('<foo><bar><baz>Bar')
c.expect_exact(':: UNFINISHED: <foo><bar><baz>Bar</baz></bar></foo>')
c.sendline('Text</baz>')
c.expect_exact(':: UNFINISHED: <foo><bar><baz>BarText</baz></bar></foo>')
c.sendline('</bar>Some other text</fo')
c.expect_exact(':: UNFINISHED: <foo><bar><baz>BarText</baz></bar>Some other text</foo>')
c.sendline('o>')
c.expect_exact(':: FINISHED: <foo><bar><baz>BarText</baz></bar>Some other text</foo>')
c.send(EOF)
