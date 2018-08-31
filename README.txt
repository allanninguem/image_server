1 - Before run make, set enviroments by running

	source ../my_set_puregev_env

	This is a custom version of the /opt/pleora/ebus_sdk/RHEL-CentOS-7-x86_64/bin/set_puregev_env because the original were not detecting the DIRNAME correctly

	Remember to check if DIRNAME (the SDK base folder) is correct (it might depend on the instalation of the SDK). Edit my_set_puregev_env if necessary


2 - Remember to open 6660 port
	
	sudo firewall-cmd --permanent --add-port=6660/tcp

