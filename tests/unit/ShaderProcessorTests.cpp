#include <gtest/gtest.h>
#include "graphics/ShaderProcessor.h"
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace cilantro;

namespace {

class ShaderProcessorTest : public ::testing::Test
{
protected:
    void SetUp () override
    {
        auto* info = ::testing::UnitTest::GetInstance ()->current_test_info ();
        m_dir = std::filesystem::temp_directory_path () / (std::string ("cilantro_shader_test_") + info->test_suite_name () + "_" + info->name ());
        std::filesystem::remove_all (m_dir);
        std::filesystem::create_directories (m_dir);
    }

    void TearDown () override
    {
        std::filesystem::remove_all (m_dir);
    }

    // write a file to the test directory and return its path
    std::string Write (const std::string& name, const std::string& content)
    {
        std::filesystem::path path = m_dir / name;
        std::ofstream file (path, std::ios::binary);
        file << content;
        return path.generic_string ();
    }

    std::filesystem::path m_dir;
};

using ShaderProcessorDeathTest = ShaderProcessorTest;

} // namespace

TEST_F (ShaderProcessorTest, PlainSourceIsCopiedVerbatim)
{
    std::unordered_map<std::string, std::string> globals;
    ShaderProcessor processor (globals);

    std::string source = "#version 460 core\nvoid main ()\n{\n    float x = 10 % 3;\n}\n";

    EXPECT_EQ (processor.ProcessShader (Write ("plain.glsl", source)), source);
}

TEST_F (ShaderProcessorTest, EmptyFileProducesEmptyOutput)
{
    std::unordered_map<std::string, std::string> globals;
    ShaderProcessor processor (globals);

    EXPECT_EQ (processor.ProcessShader (Write ("empty.glsl", "")), "");
}

TEST_F (ShaderProcessorTest, SinglePercentSignIsNotADirective)
{
    std::unordered_map<std::string, std::string> globals;
    ShaderProcessor processor (globals);

    std::string source = "int a = 7 % 2; // 100%\n";

    EXPECT_EQ (processor.ProcessShader (Write ("percent.glsl", source)), source);
}

TEST_F (ShaderProcessorTest, GlobalIsSubstituted)
{
    std::unordered_map<std::string, std::string> globals = { { "MAX_LIGHTS", "32" } };
    ShaderProcessor processor (globals);

    std::string result = processor.ProcessShader (Write ("global.glsl", "const int n = %%MAX_LIGHTS%%;\n"));

    EXPECT_EQ (result, "const int n = 32;\n");
}

TEST_F (ShaderProcessorTest, WhitespaceInsideDirectiveIsIgnored)
{
    std::unordered_map<std::string, std::string> globals = { { "NAME", "value" } };
    ShaderProcessor processor (globals);

    std::string result = processor.ProcessShader (Write ("spaces.glsl", "[%%   NAME \t%%]"));

    EXPECT_EQ (result, "[value]");
}

TEST_F (ShaderProcessorTest, SameGlobalCanBeUsedManyTimes)
{
    std::unordered_map<std::string, std::string> globals = { { "N", "4" } };
    ShaderProcessor processor (globals);

    std::string result = processor.ProcessShader (Write ("many.glsl", "%%N%% + %%N%% = 2 * %%N%%"));

    EXPECT_EQ (result, "4 + 4 = 2 * 4");
}

TEST_F (ShaderProcessorTest, GlobalValuesMayBeMultiline)
{
    std::unordered_map<std::string, std::string> globals = { { "BODY", "a;\nb;\n" } };
    ShaderProcessor processor (globals);

    std::string result = processor.ProcessShader (Write ("multiline.glsl", "{\n%%BODY%%}\n"));

    EXPECT_EQ (result, "{\na;\nb;\n}\n");
}

TEST_F (ShaderProcessorTest, IncludeInsertsFileContent)
{
    std::unordered_map<std::string, std::string> globals;
    ShaderProcessor processor (globals);

    std::string header = Write ("header.glsl", "// header\n");
    std::string main = Write ("main.glsl", "%%include " + header + "%%void main () {}\n");

    EXPECT_EQ (processor.ProcessShader (main), "// header\nvoid main () {}\n");
}

TEST_F (ShaderProcessorTest, IncludedFileIsProcessedRecursively)
{
    std::unordered_map<std::string, std::string> globals = { { "COUNT", "8" } };
    ShaderProcessor processor (globals);

    std::string leaf = Write ("leaf.glsl", "leaf(%%COUNT%%)");
    std::string middle = Write ("middle.glsl", "middle[%%include " + leaf + "%%]");
    std::string main = Write ("main.glsl", "main{%%include " + middle + "%%}");

    EXPECT_EQ (processor.ProcessShader (main), "main{middle[leaf(8)]}");
}

TEST_F (ShaderProcessorTest, MultipleDifferentIncludesAreAllExpanded)
{
    std::unordered_map<std::string, std::string> globals;
    ShaderProcessor processor (globals);

    std::string a = Write ("a.glsl", "A");
    std::string b = Write ("b.glsl", "B");
    std::string main = Write ("main.glsl", "%%include " + a + "%%-%%include " + b + "%%");

    EXPECT_EQ (processor.ProcessShader (main), "A-B");
}

TEST_F (ShaderProcessorTest, ResetAllowsProcessingTheSameFileAgain)
{
    std::unordered_map<std::string, std::string> globals;
    ShaderProcessor processor (globals);

    std::string file = Write ("again.glsl", "content");

    EXPECT_EQ (processor.ProcessShader (file), "content");
    processor.Reset ();
    EXPECT_EQ (processor.ProcessShader (file), "content");
}

TEST_F (ShaderProcessorTest, GlobalsAreLookedUpAtProcessingTime)
{
    // the processor keeps a reference to the map, so later changes are visible
    std::unordered_map<std::string, std::string> globals = { { "V", "1" } };
    ShaderProcessor processor (globals);
    std::string file = Write ("live.glsl", "%%V%%");

    EXPECT_EQ (processor.ProcessShader (file), "1");

    globals["V"] = "2";
    processor.Reset ();
    EXPECT_EQ (processor.ProcessShader (file), "2");
}

// Shader preprocessing errors are fatal: the error is logged and the process exits with EXIT_FAILURE

TEST_F (ShaderProcessorDeathTest, MissingFileIsFatal)
{
    std::unordered_map<std::string, std::string> globals;
    ShaderProcessor processor (globals);
    std::string missing = (m_dir / "does_not_exist.glsl").generic_string ();

    EXPECT_EXIT (processor.ProcessShader (missing), ::testing::ExitedWithCode (EXIT_FAILURE), "");
}

TEST_F (ShaderProcessorDeathTest, UnknownGlobalIsFatal)
{
    std::unordered_map<std::string, std::string> globals;
    ShaderProcessor processor (globals);
    std::string file = Write ("unknown.glsl", "%%NOT_DEFINED%%");

    EXPECT_EXIT (processor.ProcessShader (file), ::testing::ExitedWithCode (EXIT_FAILURE), "");
}

TEST_F (ShaderProcessorDeathTest, UnterminatedDirectiveIsFatal)
{
    std::unordered_map<std::string, std::string> globals = { { "X", "1" } };
    ShaderProcessor processor (globals);
    std::string file = Write ("unterminated.glsl", "%%X");

    EXPECT_EXIT (processor.ProcessShader (file), ::testing::ExitedWithCode (EXIT_FAILURE), "");
}

TEST_F (ShaderProcessorDeathTest, IncludeWithoutFilenameIsFatal)
{
    std::unordered_map<std::string, std::string> globals;
    ShaderProcessor processor (globals);
    std::string file = Write ("noname.glsl", "%%include%%");

    EXPECT_EXIT (processor.ProcessShader (file), ::testing::ExitedWithCode (EXIT_FAILURE), "");
}

TEST_F (ShaderProcessorDeathTest, CircularIncludeIsFatal)
{
    std::unordered_map<std::string, std::string> globals;
    ShaderProcessor processor (globals);

    std::string a = (m_dir / "a.glsl").generic_string ();
    std::string b = (m_dir / "b.glsl").generic_string ();
    Write ("a.glsl", "%%include " + b + "%%");
    Write ("b.glsl", "%%include " + a + "%%");

    EXPECT_EXIT (processor.ProcessShader (a), ::testing::ExitedWithCode (EXIT_FAILURE), "");
}
