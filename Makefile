obj-y += server.o

all: server
# Dummy command so that make thinks it has done something
	@true

server: $(obj-y)
	$(call LINK, $^)

include rules.mak
