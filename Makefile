.PHONY: run test assets

STAGE ?=

run:
	./gradlew run $(if $(STAGE),-Dmmx.stage=$(STAGE))

test:
	./gradlew test

assets:
	python3 tools/import_assets.py
