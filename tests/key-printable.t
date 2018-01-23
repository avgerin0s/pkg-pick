description: printable characters are added to the query
keys: A \n # ENTER
stdin:
BBB
ABC
AB
A
stdout:
A

description: printable UTF-8 characters are added to the query
keys: ö \n # ENTER
stdin:
å
ä
ö
stdout:
ö

description: UTF-8 false positive
keys: áá \n # ENTER
stdin:
íš á
háh háh
stdout:
háh háh

description: UTF-8 four byte wide characters
keys: 💩 \n # ENTER
stdin:
😀
💩
stdout:
💩

description: changing the query resets vertical scroll
keys: \016 \016 \016 \016 \016 0 \n #DOWN ENTER
env: LINES=5
stdin:
01
02
03
04
05
stdout:
01
