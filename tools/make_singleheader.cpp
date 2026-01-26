#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#define SEPARATOR(name) \
    "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n" \
    "// " name "\n" \
    "////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\n\n"               

typedef struct content_node_t
{
    unsigned int start;
    unsigned int end;
    const char* filePath;
} content_node;

void fprintf_content_node(content_node* node, FILE* output)
{
    FILE* file = fopen(node->filePath, "r+");
    if (!file) return;

    char buffer[1024];
    int currentLine = 0;
    
    while (currentLine < node->end) {
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, sizeof(buffer), file);

        if (currentLine >= node->start && currentLine < node->end) {
            fprintf(output, "%s", buffer);
        }
        currentLine++;
    }

    fprintf(output, "\n");
    fclose(file);
}

void fun_create_header(void)
{
    char header[] = 
    {
        "#ifndef CTOOLBOX_INCLUDED\n"
        "#define CTOOLBOX_INCLUDED\n\n"
        "#include <stddef.h>\n"
        "#include <stdint.h>\n"
        "#include <stdbool.h>\n\n"
    };

    content_node_t context; context.start = 7; context.end = 78; context.filePath = "../context.h";
    content_node_t darray; darray.start = 5; darray.end = 71; darray.filePath = "../darray.h";
    content_node_t idgen; idgen.start = 5; idgen.end = 39; idgen.filePath = "../idgen.h";
    content_node_t shashtable; shashtable.start = 5; shashtable.end = 52; shashtable.filePath = "../shashtable.h";

    char footer[] = 
    {
        "#endif // CTOOLBOX_INCLUDED\n\n"
        "#ifdef CTOOLBOX_IMPLEMENTATION\n"
        "#undef CTOOLBOX_IMPLEMENTATION\n"
        "#include \"ctoolbox.c\"\n"
        "#endif // CTOOLBOX_IMPLEMENTATION\n"
    };

    FILE* outputFile = fopen("../headeronly/ctoolbox.h", "w");

    fprintf(outputFile, "%s", header);

    fprintf(outputFile, "%s", SEPARATOR("Context"));
    fprintf_content_node(&context, outputFile);

    fprintf(outputFile, "%s", SEPARATOR("Dynamic Array"));
    fprintf_content_node(&darray, outputFile);

    fprintf(outputFile, "%s", SEPARATOR("ID Generator"));
    fprintf_content_node(&idgen, outputFile);

    fprintf(outputFile, "%s", SEPARATOR("Static Hashtable"));
    fprintf_content_node(&shashtable, outputFile);

    fprintf(outputFile, "%s", footer);

    fclose(outputFile);
}

void fun_create_source(void)
{
    char header[] = 
    {
        "#include \"ctoolbox.h\"\n"
        "#include <stdlib.h>\n"
        "#include <string.h>\n\n"
    };

    FILE* outputFile = fopen("../headeronly/ctoolbox.c", "w");

    content_node_t context; context.start = 3; context.end = 31; context.filePath = "../context.c";
    content_node_t darray; darray.start = 5; darray.end = 233; darray.filePath = "../darray.c";
    content_node_t idgen; idgen.start = 5; idgen.end = 159; idgen.filePath = "../idgen.c";
    content_node_t shashtable; shashtable.start = 4; shashtable.end = 180; shashtable.filePath = "../shashtable.c";

    fprintf(outputFile, "%s", header);

    fprintf(outputFile, "%s", SEPARATOR("Context"));
    fprintf_content_node(&context, outputFile);

    fprintf(outputFile, "%s", SEPARATOR("Dynamic Array"));
    fprintf_content_node(&darray, outputFile);

    fprintf(outputFile, "%s", SEPARATOR("ID Generator"));
    fprintf_content_node(&idgen, outputFile);

    fprintf(outputFile, "%s", SEPARATOR("Static Hashtable"));
    fprintf_content_node(&shashtable, outputFile);

    fclose(outputFile);
}

int main(void)
{
    fun_create_header();
    fun_create_source();

    return 0;
}