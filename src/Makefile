CPPFLAGS2=$(CPPFLAGS) -I$(shell pwd)
LIBS=-lm
primeOthello: board.o ai.o value.o gameTree.o dfs.o
	$(CPPC) $(CPPFLAGS2) -o primeOthello ai.o gameTree.o dfs.o value.o board.o $(LIBS)
ai.o: AI/ai.cpp
	$(CPPC) $(CPPFLAGS2) -o ai.o -c AI/ai.cpp
value.o: AI/ai.cpp
	$(CPPC) $(CPPFLAGS2) -o value.o -c AI/value.cpp
gameTree.o: AI/gameTree.cpp
	$(CPPC) $(CPPFLAGS2) -o gameTree.o -c AI/gameTree.cpp
dfs.o: AI/dfs.cpp
	$(CPPC) $(CPPFLAGS2) -o dfs.o -c AI/dfs.cpp
board.o: Board/board.cpp
	$(CPPC) $(CPPFLAGS2) -o board.o -c Board/board.cpp
server.o: GameServer/server.cpp GameServer/othelloAI.cpp
	$(CPPC) $(CPPFLAGS2) -o server.o -c GameServer/server.cpp
gametimer.o: GameServer/gametimer.cpp
	$(CPPC) $(CPPFLAGS2) -o gametimer.o -c GameServer/gametimer.cpp
TestGameState:
	make -C Board all CPPC='$(CPPC)' CPPFLAGS='$(CPPFLAGS2)'
	mv Board/*.o .
	make -C Test all CPPC='$(CPPC)' CPPFLAGS='$(CPPFLAGS2)'
	mv Test/*.o .
	$(CPPC) $(CPPFLAGS2) -o TestGameState Test.o GameTree.o GameState.o basic_GameState.o LineState.o Base3.o Position.o State.o $(LIBS)
othelloAI.o: GameServer/othelloAI.cpp
	$(CPPC) $(CPPFLAGS2) -o othelloAI.o -c GameServer/othelloAI.cpp
ogs: board.o server.o gametimer.o othelloAI.o
	$(CPPC) $(CPPFLAGS2) -o ogs server.o gametimer.o othelloAI.o board.o $(LIBS)
ogs2:
	make -C Board all CPPC='$(CPPC)' CPPFLAGS='$(CPPFLAGS2)'
	mv Board/*.o .
	make -C GameServer2 all CPPC='$(CPPC)' CPPFLAGS='$(CPPFLAGS2)'
	mv GameServer2/*.o .
	$(CPPC) $(CPPFLAGS2) -o ogs Server.o GameTimer.o OthelloAI.o GameState.o $(LIBS)
rndo: GameServer/random.cpp board.o
	$(CPPC) $(CPPFLAGS2) -o rndo board.o GameServer/random.cpp $(LIBS)
clean:
	-rm *.o
