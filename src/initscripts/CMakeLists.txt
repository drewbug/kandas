if(SELECT_INITSCRIPT MATCHES suse)
	install(PROGRAMS kandas-daemon.suse DESTINATION /etc/init.d RENAME kandas-daemon)
endif(SELECT_INITSCRIPT MATCHES suse)
