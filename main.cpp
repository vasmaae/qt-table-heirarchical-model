#include <QApplication>
#include <QTableView>
#include <QAbstractTableModel>
#include <QVBoxLayout>
#include <QWidget>
#include <QHeaderView>
#include <QMouseEvent>

// Кастомная модель для таблицы с иерархическим поведением
class HierarchicalTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    struct Item {
        QString name;
        QString value;
        bool isHeader;
        bool isExpanded;
        QList<Item> children; // Дочерние элементы для заголовка
    };

    HierarchicalTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent) {}

    // Реализация необходимых методов модели
    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        return items.size();
    }

    int columnCount(const QModelIndex& parent = QModelIndex()) const override {
        return 2; // Два столбца: Name и Value
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || role != Qt::DisplayRole)
            return QVariant();

        const Item& item = items[index.row()];
        if (index.column() == 0)
            return item.name;
        else
            return item.value;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            return section == 0 ? "Name" : "Value";
        }
        return QVariant();
    }

    // Добавление заголовка
    void addHeader(const QString& name) {
        beginInsertRows(QModelIndex(), items.size(), items.size());
        items.append({name, "Header Value", true, false, {}});
        endInsertRows();
    }

    // Добавление данных под заголовок
    void addDataUnderHeader(int headerIndex, const QString& name, const QString& value) {
        if (headerIndex >= 0 && headerIndex < items.size()) {
            items[headerIndex].children.append({name, value, false, false, {}});
        }
    }

    // Обработка клика по заголовку
    void toggleHeader(int row) {
        if (row < 0 || row >= items.size() || !items[row].isHeader)
            return;

        Item& header = items[row];
        if (header.isExpanded) {
            // Сворачиваем: удаляем дочерние строки
            int childCount = header.children.size();
            if (childCount > 0) {
                beginRemoveRows(QModelIndex(), row + 1, row + childCount);
                for (int i = 0; i < childCount; ++i) {
                    items.removeAt(row + 1);
                }
                header.isExpanded = false;
                endRemoveRows();
            }
        } else {
            // Разворачиваем: вставляем дочерние строки
            beginInsertRows(QModelIndex(), row + 1, row + header.children.size());
            for (const auto& child : header.children) {
                items.insert(row + 1, child);
            }
            header.isExpanded = true;
            endInsertRows();
        }
    }

private:
    QList<Item> items; // Список всех элементов (заголовки и данные)
};

// Кастомный QTableView для обработки кликов
class HierarchicalTableView : public QTableView {
    Q_OBJECT
public:
    HierarchicalTableView(QWidget* parent = nullptr) : QTableView(parent) {}

protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            QModelIndex index = indexAt(event->pos());
            if (index.isValid()) {
                auto* model = dynamic_cast<HierarchicalTableModel*>(this->model());
                if (model) {
                    model->toggleHeader(index.row());
                }
            }
        }
        QTableView::mousePressEvent(event);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Создаем главное окно
    QWidget window;
    QVBoxLayout* layout = new QVBoxLayout(&window);

    // Создаем модель
    HierarchicalTableModel* model = new HierarchicalTableModel(&window);

    // Заполняем модель данными
    model->addHeader("Category 1");
    model->addDataUnderHeader(0, "Item 1.1", "Value 1.1");
    model->addDataUnderHeader(0, "Item 1.2", "Value 1.2");

    model->addHeader("Category 2");
    model->addDataUnderHeader(1, "Item 2.1", "Value 2.1");
    model->addDataUnderHeader(1, "Item 2.2", "Value 2.2");
    model->addDataUnderHeader(1, "Item 2.3", "Value 2.3");

    // Создаем кастомный QTableView
    HierarchicalTableView* tableView = new HierarchicalTableView(&window);
    tableView->setModel(model);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setShowGrid(true);

    // Добавляем таблицу в layout
    layout->addWidget(tableView);

    // Настраиваем окно
    window.setWindowTitle("Hierarchical Table");
    window.resize(400, 300);
    window.show();

    return app.exec();
}

#include "main.moc"
