kate-links
==========

Plugin for KTextEditor (and Kate) which aim is to highlight links in document and make them clickable.

Building kate-links plugin from source:
--------------------------------------------------

Simply write those commands in kate-links folder:<br />
$ mkdir build<br />
$ cd build<br />
$ cmake .. -DCMAKE\_INSTALL\_PREFIX=/usr<br />
$ make<br />
$ sudo make install<br />

_-DCMAKE\_INSTALL\_PREFIX_ option defines the path where kate-links will be installed.
It should be the same as KDE installation path (usually "/usr" is ok).
