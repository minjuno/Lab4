#include <gtk/gtk.h>

double first_operand = 0.0;
double second_operand = 0.0;
char operation = '\0';

static void on_button_clicked(GtkWidget *widget, gpointer data) {

    const gchar *button_label = gtk_button_get_label(GTK_BUTTON(widget));

    GtkWidget *entry = GTK_WIDGET(data);
    const gchar *current_text = gtk_entry_get_text(GTK_ENTRY(entry));

    if (g_ascii_isdigit(button_label[0])) {
        // 숫자 버튼인 경우
        gchar *new_text = g_strdup_printf("%s%s", current_text, button_label);
        gtk_entry_set_text(GTK_ENTRY(entry), new_text);
        g_free(new_text);
    } else if (button_label[0] == 'C') {
        // 'C' 버튼인 경우 (초기화)
        gtk_entry_set_text(GTK_ENTRY(entry), "");
        first_operand = 0.0;
        second_operand = 0.0;
        operation = '\0';
    } else if (button_label[0] == '=') {
        // '=' 버튼인 경우 (계산 수행)
        second_operand = atof(current_text);

        // 연산 수행
        double result = 0.0;
        switch (operation) {
            case '+':
                result = first_operand + second_operand;
                break;
	    case '-':
		result = first_operand - second_operand;
		break;
	    case '*':
		result = first_operand * second_operand;
	        break;
	    case '/':
		result = first_operand / second_operand;
		break;
        }

        gchar *result_text = g_strdup_printf("%.2f", result);
        gtk_entry_set_text(GTK_ENTRY(entry), result_text);
        g_free(result_text);

        first_operand = result;
        operation = '\0';
    } else {
        first_operand = atof(current_text);
        operation = button_label[0];

        gtk_entry_set_text(GTK_ENTRY(entry), "");
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // 윈도우 생성
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "juno's calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 210, 210);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 4, 1);

    // 버튼 생성 및 배치
    gchar *button_labels[] = {"7", "8", "9", "/",
                              "4", "5", "6", "*",
                              "1", "2", "3", "-",
                              "0", "C", "=", "+"};
    for (int i = 0; i < 16; ++i) {
        GtkWidget *button = gtk_button_new_with_label(button_labels[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, i / 4 + 1, 1, 1);
    }

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

