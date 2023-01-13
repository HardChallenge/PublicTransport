compile:
	g++ -o s server.cpp -std=c++17
	g++ Transport.cpp -I /opt/homebrew/Cellar/sfml/2.5.1_2/include -o t -L /opt/homebrew/Cellar/sfml/2.5.1_2/lib -lsfml-graphics -lsfml-window -lsfml-system -std=c++17
	g++ -o c client.cpp -std=c++17

clean:
	rm -f s c t
	clear