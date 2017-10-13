server-objs += server.o
client-objs += client.o

all: server client
# Dummy command so that make thinks it has done something
	@true

server: $(server-objs)
	$(call LINK, $^)

client: $(client-objs)
	$(call LINK, $^)

include rules.mak
