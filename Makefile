CXX := g++
CXXFLAGS := -std=c++17 -O2
INCLUDES := -I/opt/homebrew/opt/openssl@3/include
LDFLAGS := -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto

# 通用源文件
COMMON_SRC := BigNumber.cpp
KEYGEN_SRC := GenerateKey.cpp
RSA_SRC := RsaCrypto.cpp

# 目标
TARGETS := step1_test step2_test step3_test step4_test

# 编译规则
all: $(TARGETS)

step1_test: step1.cpp $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) $^ $(INCLUDES) $(LDFLAGS) -o $@

step2_test: step2.cpp $(COMMON_SRC) $(KEYGEN_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

step3_test: step3.cpp $(COMMON_SRC) $(KEYGEN_SRC) $(RSA_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

step4_test: step4.cpp $(COMMON_SRC) $(KEYGEN_SRC)
	$(CXX) -std=c++17 -O3 -flto -march=native $^ -o $@

test: all
	@echo "Running step1_test..."
	@./step1_test
	@echo "Running step2_test..."
	@./step2_test
	@echo "Running step3_test..."
	@./step3_test
	@echo "Running step4_test..."
	@./step4_test

clean:
	rm -f $(TARGETS)
