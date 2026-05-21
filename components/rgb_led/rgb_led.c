#include "rgb_led.h"
#include "esp_err.h"
#include "led_strip.h"

static color_t last_color = {0, 0, 0};
static uint8_t brightness = 255;

/**
 * @brief Aplica el factor de brillo al color base y actualiza el hardware.
 * @note Operación interna: (color * brillo) / 255.
 *
 * @param led Puntero al controlador del LED strip.
 * @return ESP_OK en éxito, o error si explota.
 */
static esp_err_t led_update_hardware(led_strip_t *led) {
    uint8_t r = (last_color.r * brightness) / 255;
    uint8_t g = (last_color.g * brightness) / 255;
    uint8_t b = (last_color.b * brightness) / 255;

    esp_err_t err = led->set_pixel(led, 0, r, g, b);
    if (err != ESP_OK)
        return err;

    return led->refresh(led, 100);
}

/**
 * @brief Define el color base del LED y actualiza su estado físico.
 *
 * @param led   Puntero al controlador del LED strip.
 * @param color El color xd.
 * @return ESP_OK si se actualizó correctamente.
 */
esp_err_t led_set_color(led_strip_t *led, color_t color) {
    last_color = color;
    return led_update_hardware(led);
}

/**
 * @brief Incrementa el nivel de brillo global.
 *
 * @param led  Puntero al controlador del LED strip.
 * @param step Cantidad a incrementar (0-255). Satura en 255.
 * @return ESP_OK luego de hacer refresh al LED.
 */
esp_err_t led_brightness_up(led_strip_t *led, uint8_t step) {
    if (255 - brightness < step) {
        brightness = 255;
    } else {
        brightness += step;
    }
    return led_update_hardware(led);
}

/**
 * @brief Decrementa el nivel de brillo global.
 *
 * @param led  Puntero al controlador del LED strip.
 * @param step Cantidad a decrementar (0-255). Se satura en 0.
 * @return ESP_OK luego de hacer refresh al LED.
 */
esp_err_t led_brightness_down(led_strip_t *led, uint8_t step) {
    if (brightness < step) {
        brightness = 0;
    } else {
        brightness -= step;
    }
    return led_update_hardware(led);
}

/**
 * @brief Enciende el LED con el último color y brillo configurados.
 *
 * @param led Puntero al controlador del LED strip.
 * @return ESP_OK si el hardware respondió correctamente.
 */
esp_err_t led_on(led_strip_t *led) {
    return led_update_hardware(led);
}

/**
 * @brief Apaga el LED físicamente sin borrar la configuración de color o brillo.
 *
 * @param led Puntero al controlador del LED strip.
 * @return ESP_OK si se apagó correctamente.
 */
esp_err_t led_off(led_strip_t *led) {
    esp_err_t err = led->set_pixel(led, 0, 0, 0, 0);
    if (err != ESP_OK)
        return err;

    return led->refresh(led, 100);
}

/**
 * @brief Función wrapper a ruego del Dr. Ing. Nicolás Alberto Calarco
 * No funciona, no use. Solo valor teórico
 */
esp_err_t create_led(led_strip_t *led) {
    return led_rgb_init(&led);
}
