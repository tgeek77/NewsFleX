# Original Description

```
Begin3
Title:          NewsFleX
Version:        1.1.9.5
Entered-date:   04Oct2000
Description:    NNTP offline newsreader with GUI
                Build in SMTP mail, can also make use of sendmail,
                URL list, can get web pages for reading offline.
                To unpack:
                tar -zxvf NewsFleX-1.1.9.5.tgz
                cd NewsFleX-1.1.9.5
                Then follow the instructions in README.RELEASE.
                Note: This software makes use of the xforms GUI library,
                xforms can be found at: http://world.std.com/~xforms/
                xforms is free for non commercial use.
                Without xforms.088 or 089 installed it will not run.
                Changes:
                Static compiled executable included, compiling
                on RedHat 6.1 (or using xforms glibc) works,
		Bugfixes with thanks to Boris Jakubith and alfonsmc.
                Now you can still use NewsFlex, by using the precompiled
                version included with this archive.
                Sorted display in newsserver period list.
                Changes from 1.1.9.4:
                posting source header, cut sig in reply, beep on mail.
Keywords:       NNTP, offline, NEWS, news reader, Free Agent, HTTP, SMTP, FTP,
                xagent, xforms
Author:         jan@panteltje.demon.nl
Maintained-by:  jan@panteltje.demon.nl
Primary-site:   http://www.panteltje.demon.nl/newsflex/
Alternate-site: ftp sunsite.unc.edu/pub/linux/system/news/readers/
                http://www.home.zonnet.nl/panteltje/newsflex/
Platforms:	LINUX, UNIX
Copying-policy: GPL
End
```

# Old README

I here by release NewsFleX (a further development of xagent) under the GPL.
I have disabled the expiration date test in shareware.c, so no online limitation apply.

It needs xforms.088 or .89 to run.
xforms is free for non commercial use.

The newsreader has proved itself very useful to me.
hope you like it.

Configuration:
maybe edit the top of the Imakefile for directories, or accept the defaults

To make it:
`xmkmf`
`make`

To install:
`make install`


You need to copy newsflexuser.tgz to your home directory and do
`tar -kzxvf newsflexuser.tgz` in your home dir, this creates the configuration
files.

start the program by typing:
`NewsFleX`

Please read the other files.

If you compile on libc, remove the -lcrypt in the Imakefile before
you compile.




# Archivist Notes

* This project was originally found at http://www.panteltje.demon.nl/newsflex/ ([archive](https://web.archive.org/web/20010812013234/http://www.panteltje.demon.nl/newsflex/)).
* Created bin directory to separate the binary file, NewsFleX-1.1.5.static, from the source code. NewsFleX-1.1.5.static is 32-bit and requires the ability to run 32-bit programs.
* Fixed formatting
