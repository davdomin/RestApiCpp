#include <iostream>
#include <string>
#include <curl/curl.h>

// Estructura de datos para almacenar la respuesta de la solicitud HTTP
struct MemoryStruct {
    char* memory;
    size_t size;
};

// Función para escribir los datos recibidos en la respuesta a la memoria
size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == nullptr) {
        std::cerr << "No se pudo asignar memoria" << std::endl;
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

int main() {
    CURL* curl;
    CURLcode res;

    // Inicializar la biblioteca cURL
    curl = curl_easy_init();
    if (curl) {
        struct MemoryStruct chunk;
        chunk.memory = (char*)malloc(1); // Inicializar la memoria
        chunk.size = 0;

        // URL del servicio REST que deseas consumir
        const char* url = "https://api.ejemplo.com/data";

        // Configurar la solicitud HTTP GET
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        // Realizar la solicitud HTTP
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Error en la solicitud cURL: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Imprimir la respuesta
            std::cout << "Respuesta del servicio web:" << std::endl;
            std::cout << chunk.memory << std::endl;
        }

        // Liberar la memoria utilizada por cURL
        curl_easy_cleanup(curl);

        // Liberar la memoria utilizada para almacenar la respuesta
        free(chunk.memory);
    }

    return 0;
}
 