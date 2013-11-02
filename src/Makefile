CPPFLAGS2=$(CPPFLAGS) -I$(shell pwd)
LIBS=-lm
primeOthello: board.o ai.o value.o
	$(CPPC) $(CPPFLAGS2) -o primeOthello ai.o value.o board.o $(LIBS)
ai.o: AI/ai.cpp
	$(CPPC) $(CPPFLAGS2) -o ai.o -c AI/ai.cpp
value.o: AI/ai.cpp
	$(CPPC) $(CPPFLAGS2) -o value.o -c AI/value.cpp
board.o: Board/board.cpp
	$(CPPC) $(CPPFLAGS2) -o board.o -c Board/board.cpp
server.o: GameServer/server.cpp GameServer/othelloAI.cpp
	$(CPPC) $(CPPFLAGS2) -o server.o -c GameServer/server.cpp
gametimer.o: GameServer/gametimer.cpp
	$(CPPC) $(CPPFLAGS2) -o gametimer.o -c GameServer/gametimer.cpp
othelloAI.o: GameServer/othelloAI.cpp
	$(CPPC) $(CPPFLAGS2) -o othelloAI.o -c GameServer/othelloAI.cpp
ogs: board.o server.o gametimer.o othelloAI.o
	$(CPPC) $(CPPFLAGS2) -o ogs server.o gametimer.o othelloAI.o board.o $(LIBS)
rndo: GameServer/random.cpp board.o
	$(CPPC) $(CPPFLAGS2) -o rndo board.o GameServer/random.cpp $(LIBS)
clean:
	-rm primeOthello
	-rm ogs
	-rm rndo
	-rm *.o
