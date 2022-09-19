#ifndef W4OD_H
#define W4OD_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtWidgets>
#include <QTimer>
#include <QPalette>
#include <unistd.h>


namespace Ui {
class w4od;
}

class w4od : public QMainWindow
{
    Q_OBJECT

public:
    explicit w4od(QWidget *parent = 0);
    ~w4od();

private slots:

    void on_Received_odrv_2_Data();

    void on_Received_odrv_3_Data();

    void read_tof();

    void parse_tof();

    void on_forward_pushButton_pressed();

    void on_forward_pushButton_released();

    void forward_released();

    void on_reverse_pushButton_pressed();

    void on_reverse_pushButton_released();

    void reverse_released();

    void on_speed_doubleSpinBox_valueChanged(double arg1);

    void on_left_pushButton_pressed();

    void left_pressed();

    void on_left_pushButton_released();

    void left_released();

    void on_right_pushButton_pressed();

    void right_pressed();

    void on_right_pushButton_released();

    void right_released();

    void on_pivot_starboard_pushButton_pressed();

    void on_pivot_port_pushButton_pressed();

    void on_pivot_port_pushButton_released();

    void on_pivot_starboard_pushButton_released();

    void on_show_volt_pushButton_clicked();

    void on_closed_loop_pushButton_clicked();

    void on_idle_pushButton_clicked();

    void on_cal_encoder_pushButton_clicked();

    void on_turn_ratio_doubleSpinBox_valueChanged(double arg1);

    void on_pivot_spd_doubleSpinBox_valueChanged(double arg1);

    void on_checkBox_stateChanged(int arg1);

    void on_sys_info_pushButton_clicked();

    void first_info();

    void second_info();

    void on_speed_constant_checkBox_clicked(bool checked);

    void on_avoidance_checkBox_clicked(bool checked);

    void timed_reverse_release();

    void timed_uturn();

    void on_unjam_pushButton_clicked();





    void on_reboot_odrv_pushButton_clicked();

private:
    int unjam_cnt = 0;
    int fport=0;            // forward port value
    double front=0;            // front tof value
    int fstarboard=0;       // forward starboard value
    double frontstop = 100;    // value to stop at



    float steer_divisor=2.5;

    float pvt_spd = 0.25;
    float velocity_bu = 0.25;
    float velocity0;   //velocity of motor0
    float velocity1;   //velocity of motor1

    QSerialPort *odrv_2;
    QSerialPort *odrv_3;
    QSerialPort *tof;

    QByteArray motor0;
    QByteArray motor1;

    QByteArray serial_tof_data;
    QByteArray read_ba;

    static const quint16 odrv_2_vendor_id = 4617;  // 0x1209
    static const quint16 odrv_2_product_id = 3378; //0xd32
    static const quint16 odrv_3_vendor_id = 4617;  // 0x1209
    static const quint16 odrv_3_product_id = 3378; //0xd32
    static const quint16 stm32_vendor_id = 1155;  // tof stm32duino
    static const quint16 stm32_product_id = 14155;  // stm32 device

    // NOTE: Have change odrive ID to relate to serial numbers
    // odrv_2 will relate to serial number 2080xxxxxxxx
    // odrv_3 will relate to serial number 3675xxxxxxxx

    QString odrv_2_serial_number = "2080377E5753";  // serial id first odrive
    QString odrv_3_serial_number = "3675385C3030";  // serial id 2nd purchased odrive
    QString odrv_2_port_name;    // serial odrv2
    QString odrv_3_port_name;    // serial odrv3
    QString description_tof = "STM32 STLink";   //desc STM32 device

    bool odrv_2_is_available;    // True if odrv0 is avail
    bool odrv_3_is_available;
    bool steer_left;
    bool steer_right;
    bool front_wd_flag;          // flag for front wheel drive
    bool tof_is_available;        // true if tof is avail
    bool avoid_flag;
    bool front_unjam = false;

    QString str_pvt_spd;
    QString str_velocity;
    QString str_velocity_0;
    QString str_velocity_1;
    QString tof_name;           // name for tof

    QString tof_buffer;     // tof buffer;
    QString front_tof;      //front buffer
    QString left_tof;       // port buffer
    QString right_tof;      // strbrd buffer

    QString buffer0;
    QString buffer1;

    QStringList tof_buffer_split;
    QString wheel_pos;
    QString pos_0;
    QString pos_1;


    Ui::w4od *ui;
};

#endif // W4OD_H
