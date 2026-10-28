.PHONY: run test assets docs dist dist-linux dist-windows

STAGE ?=

run:
	./gradlew run $(if $(STAGE),-Dmmx.stage=$(STAGE))

test:
	./gradlew test

assets:
	python3 tools/import_assets.py

docs:
	python3 tools/import_docs.py

dist: dist-linux dist-windows

dist-linux:
	rm -f build/libs/*.jar
	./gradlew jar
	./tools/build_game.sh linux64

dist-windows:
	rm -f build/libs/*.jar
	./gradlew jar
	./tools/build_game.sh windows64
