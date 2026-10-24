cmake_minimum_required(VERSION 3.19)
project(FacturacionChapinas LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets PrintSupport)

qt_standard_project_setup()

qt_add_executable(FacturacionChapinas
    main.cpp
    mainwindow.cpp
    mainwindow.h
    modelos.h
    datos.h
    vendedordialog.cpp
    vendedordialog.h
    vendedoreswidget.cpp
    vendedoreswidget.h
    clientedialog.cpp
    clientedialog.h
    clienteswidget.cpp
    clienteswidget.h
    productodialog.cpp
    productodialog.h
    productoswidget.cpp
    productoswidget.h
    facturadialog.cpp
    facturadialog.h
    facturaswidget.cpp
    facturaswidget.h
    pagodialog.cpp
    pagodialog.h
    pagoswidget.cpp
    pagoswidget.h
    reporteswidget.cpp
    reporteswidget.h
)

target_link_libraries(FacturacionChapinas PRIVATE Qt6::Widgets Qt6::PrintSupport)