### DH Algorithm Description Based on UDP
In UDP(User Datagram Protocol) data segment, the first 10-bit explains the transmission type while the others express the transmission value.
`send_p_g`: represents "send p and g", the behind it is the value `p` and value `q` which are separated by comma.
`send_Ya`: represents "send Ya", the behind it is the value `Ya`.
`send_Yb`: represents "send Yb", the behind it is the value `Yb`.
Client sends `p` and `q` at first, server sends `Yb` after it receives `p` and `q`, then client sends `Ya` after `Yb`, finally client and server both get the same key `key`.
This is an overview on it (image values as colors).

<img src="overview.png" style="zoom:60%">

