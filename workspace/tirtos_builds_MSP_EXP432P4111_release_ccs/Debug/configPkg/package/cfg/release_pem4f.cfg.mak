# invoke SourceDir generated makefile for release.pem4f
release.pem4f: .libraries,release.pem4f
.libraries,release.pem4f: package/cfg/release_pem4f.xdl
	$(MAKE) -f /home/sam/workspace_v10/tirtos_builds_MSP_EXP432P4111_release_ccs/src/makefile.libs

clean::
	$(MAKE) -f /home/sam/workspace_v10/tirtos_builds_MSP_EXP432P4111_release_ccs/src/makefile.libs clean

