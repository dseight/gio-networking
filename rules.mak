include config.mak

.PHONY: all clean cscope

# Flags for dependency generation
DGFLAGS = -MMD -MP -MT $@ -MF $(@D)/$(*F).d

%.o: %.c
	$(call quiet-command, $(CC) $(INCLUDES) $(DGFLAGS) $(CFLAGS) $($@-cflags) \
	       -c -o $@ $<,"CC","$@")

%.o: %.S
	$(call quiet-command, $(AS) $(INCLUDES) $(DGFLAGS) $(CFLAGS) \
	       -c -o $@ $<,"AS","$@")

%.a:
	$(call quiet-command,rm -f $@ && $(AR) rcs $@ $^,"AR","$@")

LINK = $(call quiet-command, $(CC) $(CFLAGS) $(LDFLAGS) -o $@ $1 \
              $(LIBS),"LINK","$@")

# Usage: $(call quiet-command,command and args,"NAME","args to print")
# This will run "command and args", and print the 'quiet' output in the
# format "  NAME     args to print"
# NAME should be a short name of the command, 7 letters or fewer.
# If called with only a single argument, will print nothing in quiet mode.
quiet-command = $(if $(2),@printf "  %-7s %s\n" $2 $3 && $1, @$1)

clean:
	rm -f server
	find . -name '*.[oda]' -type f -exec rm {} +
	rm -f tags cscope.*

ctags:
	rm -f tags
	find . -name '*.[hc]' -exec ctags --append {} +

cscope:
	rm -f ./cscope.*
	find . -name "*.[chsS]" -print > ./cscope.files
	cscope -b -i./cscope.files

# Include generated dependencies
-include $(shell find . -name "*.d" -type f -print)
