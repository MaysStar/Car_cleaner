#include "esp_logs_papertrail.h"

static vprintf_like_t old_logs_uart_output;
static struct sockaddr_in dest_addr;
static int32_t sock  = -1;

static const char* TAG = "esp_logs_papertrail";

static uint32_t early_buf_idx = 0;
static char early_log_buf[EARLY_LOG_BUF_SIZE][EARLY_LOG_SIZE];

/* New hook function for logs in papertrail */
int new_hook_papertrail_function(const char *frm, va_list list)
{
    va_list list_copy;
    va_copy(list_copy, list);

    /* Send data to uart0 */
    int res = old_logs_uart_output(frm, list);

    if(sock >= 0)
    {
        for(uint32_t i = 0; i < early_buf_idx; ++i)
        {
            /* Send all data which went when wi-fi was disconected */\
            sendto(sock, early_log_buf[i], EARLY_LOG_SIZE, 0, (struct sockaddr*)& dest_addr, sizeof(dest_addr));
        }
        early_buf_idx = 0;

        /* Convert data */
        char log_buffer[256];
        int len = vsnprintf(log_buffer, sizeof(log_buffer), frm, list_copy);

        if(len > 0)
        {
            /* Redirect data into logs server */
            sendto(sock, log_buffer, strlen(log_buffer), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        }
    }
    else 
    {   
        /* If wi-fi don't connected */

        /* cycle buffer */
        early_buf_idx = (early_buf_idx >= EARLY_LOG_BUF_SIZE) ? 0 : early_buf_idx;

        /* Convert data */
        vsnprintf(early_log_buf[early_buf_idx++], EARLY_LOG_SIZE, frm, list_copy);
    }

    return res;
}

/* Inititalization logs from uart to papertrail and into uart again */
void esp_logs_papertrail_init(void)
{   
    int32_t addr_family = 0;
    int32_t ip_protocol = 0;

    /* UDP client init */
    dest_addr.sin_addr.s_addr = inet_addr(CONFIG_UDP_HOST_IP_ADDR);
    dest_addr.sin_family = AF_INET,
    dest_addr.sin_port = htons(CONFIG_UDP_HOST_PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    sock = socket(addr_family, SOCK_DGRAM, ip_protocol);

    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return;
    }

    ESP_LOGI(TAG, "UDP Socket created. Redirecting logs...");
}
/* Main task to send */
void esp_logs_papertrail_task(void* pvParameters)
{
    /* Set new hook function for logs*/
    old_logs_uart_output = esp_log_set_vprintf(new_hook_papertrail_function);

    xEventGroupWaitBits(e_tasks, WIFI_BIT_GOT_IP, pdFALSE, pdTRUE, portMAX_DELAY);
    esp_logs_papertrail_init();

    vTaskDelete(NULL);
}