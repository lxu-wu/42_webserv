#!/usr/bin/perl

# print "Content-type: text/html\n\n";
# print <<"TAG";
# <HTML>
# <HEAD>
# <TITLE></TITLE>
# </HEAD>
# <BODY>
# Your e-mail has been sent...<BR>
# Thank you for use this page realised by Moi.<BR>
# <BR>
# Tschuss!
# </BODY>
# </HTML>
# TAG



use CGI;

my $q = CGI->new;

my $lightweight_fh  = $q->upload('file1');
my $filename = $q->param('file1');

my $dir = 'server/uploads/';

my $msg;

mkdir($dir) unless (-d $dir);

# undef may be returned if it's not a valid file handle
if (defined $lightweight_fh) {
	# Upgrade the handle to one compatible with IO::Handle:
	my $io_handle = $lightweight_fh->handle;

	open (OUTFILE,'>', $dir . $filename);
	while ($bytesread = $io_handle->read($buffer,1024)) {
		print OUTFILE $buffer;
	}
	$msg = 'The file \'' . $filename . '\' was uploaded successfully with perl';
}
else {
	$msg = 'No file was uploaded';
}

print '<html><body><p>' . $msg . '</p></body></html>';