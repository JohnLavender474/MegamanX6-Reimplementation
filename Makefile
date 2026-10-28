.PHONY: run test assets docs

STAGE ?=

run:
	./gradlew run $(if $(STAGE),-Dmmx.stage=$(STAGE))

test:
	./gradlew test

assets:
	python3 tools/import_assets.py

docs:
	python3 tools/import_docs.py
