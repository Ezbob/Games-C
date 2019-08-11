BUILD_TOOL?=ninja
BUILD_TOOL_CMAKE?="Ninja"
BUILD_TOOL_FILE?=build.ninja
EXEC_NAME?=checkers

BUILD_DIR_PREFIX=build
BUILD_DIR_RELEASE=${BUILD_DIR_PREFIX}/release
BUILD_DIR_DEBUG=${BUILD_DIR_PREFIX}/debug

all: debug

debug: $(BUILD_DIR_DEBUG)/$(BUILD_TOOL_FILE)
	@cd $(BUILD_DIR_DEBUG) && ${BUILD_TOOL}

release: $(BUILD_DIR_RELEASE)/$(BUILD_TOOL_FILE)
	@cd $(BUILD_DIR_RELEASE) && ${BUILD_TOOL}

run: debug
	./${BUILD_DIR_DEBUG}/${EXEC_NAME}

$(BUILD_DIR_DEBUG)/$(BUILD_TOOL_FILE):
	@mkdir -p $(@D)
	@cd $(@D) && cmake -DCMAKE_BUILD_TYPE=Debug -DEXEC_NAME=${EXEC_NAME} -DENABLE_STATS=${STATS} -G ${BUILD_TOOL_CMAKE} ../../

$(BUILD_DIR_RELEASE)/$(BUILD_TOOL_FILE):
	@mkdir -p $(@D)
	@cd $(@D) && cmake -DCMAKE_BUILD_TYPE=Release -DEXEC_NAME=${EXEC_NAME} -DENABLE_STATS=0 -G ${BUILD_TOOL_CMAKE} ../../

clean:
	$(RM) -rf ${BUILD_DIR_PREFIX}

help:
	@echo "Available targets:"
	@echo "  - release: Builds release binary in the '${BUILD_DIR_RELEASE}' directory"
	@echo "  - debug: Builds debug binary in the '${BUILD_DIR_DEBUG}' directory"
	@echo "  - run: Builds debug binary in the '${BUILD_DIR_DEBUG}' directory and runs it"
	@echo "  - clean: Removes the build directories"
