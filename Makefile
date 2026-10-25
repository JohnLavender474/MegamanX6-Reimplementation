.PHONY: run test

STAGE ?=

run:
	./gradlew run $(if $(STAGE),-Dmmx.stage=$(STAGE))

test:
	./gradlew test
