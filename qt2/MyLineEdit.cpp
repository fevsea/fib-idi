#include "MyLineEdit.h"
#include <iostream>


MyLineEdit::MyLineEdit(QWidget *parent)
    : QLineEdit(parent){

}


void MyLineEdit::tractaReturn(){
    std::cout << text().toStdString() << std::endl;
    emit enviaText(text());
}
