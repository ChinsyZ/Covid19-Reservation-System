all: scheduler hospitalA hospitalB hospitalC  client

scheduler: scheduler.cpp
	g++ scheduler.cpp -o scheduler
hospitalA: hospitalA.cpp
	g++ hospitalA.cpp -o hospitalA
hospitalB: hospitalB.cpp
	g++ hospitalB.cpp -o hospitalB
hospitalC: hospitalC.cpp
	g++ hospitalC.cpp -o hospitalC
client: client.cpp
	g++ client.cpp -o client
clean:
	rm *.o all
