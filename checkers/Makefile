BUILD_TOOL=ninja
BUILD_TOOL_CMAKE="Ninja"
BUILD_DIR_PREFIX=build
BUILD_DIR_RELEASE=${BUILD_DIR_PREFIX}/release
BUILD_DIR_DEBUG=${BUILD_DIR_PREFIX}/debug
EXEC_NAME?=checkers
STATS?=1

all: debug

debug: $(BUILD_DIR_DEBUG)
	@cd $^ && ${BUILD_TOOL}

release: $(BUILD_DIR_RELEASE)
	@cd $^ && ${BUILD_TOOL}

$(BUILD_DIR_DEBUG):
	@mkdir -p $@
	@cd $@ && cmake -DCMAKE_BUILD_TYPE=Debug -DEXEC_NAME=${EXEC_NAME} -DENABLE_STATS=${STATS} -G ${BUILD_TOOL_CMAKE} ../../

$(BUILD_DIR_RELEASE):
	@mkdir -p $@
	@cd $@ && cmake -DCMAKE_BUILD_TYPE=Release -DEXEC_NAME=${EXEC_NAME} -DENABLE_STATS=0 -G ${BUILD_TOOL_CMAKE} ../../

run: debug
	./${BUILD_DIR_DEBUG}/${EXEC_NAME}

clean:
	$(RM) -rf ${BUILD_DIR_PREFIX}

help:
	@echo "Available targets:"
	@echo "  - release: Builds release binary in the '${BUILD_DIR_RELEASE}' directory"
	@echo "  - debug: Builds debug binary in the '${BUILD_DIR_DEBUG}' directory"
	@echo "  - run: Builds debug binary in the '${BUILD_DIR_DEBUG}' directory and runs it"
	@echo "  - clean: Removes the build directories"
