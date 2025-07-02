CC = gcc
CFLAGS = -Wall

BIN = syscleaner
SCRIPT = run_syscleaner.sh

all: $(BIN) $(SCRIPT)

$(BIN): main.o cleaner.o
	$(CC) $(CFLAGS) -o $(BIN) main.o cleaner.o

main.o: main.c cleaner.h
	$(CC) $(CFLAGS) -c main.c

cleaner.o: cleaner.c cleaner.h
	$(CC) $(CFLAGS) -c cleaner.c

$(SCRIPT):
	echo '#!/bin/bash' > $(SCRIPT); \
	echo '' >> $(SCRIPT); \
	echo 'BIN="./$(BIN)"' >> $(SCRIPT); \
	echo '' >> $(SCRIPT); \
	echo 'TERMINALS=(' >> $(SCRIPT); \
	echo '    "konsole -e"' >> $(SCRIPT); \
	echo '    "gnome-terminal --"' >> $(SCRIPT); \
	echo '    "xfce4-terminal --command"' >> $(SCRIPT); \
	echo '    "xterm -e"' >> $(SCRIPT); \
	echo '    "tilix -e"' >> $(SCRIPT); \
	echo '    "lxterminal -e"' >> $(SCRIPT); \
	echo '    "mate-terminal -e"' >> $(SCRIPT); \
	echo ')' >> $(SCRIPT); \
	echo '' >> $(SCRIPT); \
	echo 'for term in "$${TERMINALS[@]}"; do' >> $(SCRIPT); \
	echo '    CMD=$$(echo $$term | awk '\''{print $$1}'\'')' >> $(SCRIPT); \
	echo '    if command -v $$CMD &> /dev/null; then' >> $(SCRIPT); \
	echo '        echo "✅ Открываю в: $$CMD"' >> $(SCRIPT); \
	echo '        exec $$term "$$BIN"' >> $(SCRIPT); \
	echo '        exit 0' >> $(SCRIPT); \
	echo '    fi' >> $(SCRIPT); \
	echo 'done' >> $(SCRIPT); \
	echo '' >> $(SCRIPT); \
	echo 'echo "❌ Не найден терминал. Запустите вручную: $$BIN"' >> $(SCRIPT); \
	echo 'exit 1' >> $(SCRIPT); \
	chmod +x $(SCRIPT)

clean:
	rm -f *.o $(BIN) $(SCRIPT)
