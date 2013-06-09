kate-links
==========

Plugin for KTextEditor (and Kate) which aim is to highlight links in document and make them clickable.

Building kate-links plugin from source:
--------------------------------------------------

Simply write those commands in kate-links folder:
$ mkdir build
$ cd build
$ cmake .. -DCMAKE\_INSTALL\_PREFIX=/usr
$ make
$ sudo make install

-DCMAKE\_INSTALL\_PREFIX option defines the path where kate-links will be installed.
It should be the same as KDE installation path (usually "/usr" is ok).
