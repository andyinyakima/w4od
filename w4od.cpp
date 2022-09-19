// w4od field donkey odrive
// derived mostly from ddod code started in July 2022
// copy right Andy Laberge July 2022
// this code is free to use under the Qt open source license LGPLv3
// a copy of which should be in the same folder as this code.
//
// note 21 July 2022
// w4od is going to be very similar to ddod at least for a while
// because it works with Swagtron T6 motor wheels at least on the bench
// on the above date ... the T6 motors uses hall sensors instead of optical
// encoders which have much lower resolution
//


#include "w4od.h"
#include "ui_w4od.h"
#include <QDebug>

w4od::w4od(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::w4od)
{
    ui->setupUi(this);

    odrv_2_is_available = false;
        odrv_2_port_name = "";
        odrv_2 = new QSerialPort(this);

    odrv_3_is_available = false;
            odrv_3_port_name = "";
            odrv_3 = new QSerialPort(this);
        tof_is_available = false;
        tof_name = "";
            tof = new QSerialPort(this);

            tof_buffer = "";
            front_tof = "";
            left_tof = "";
            right_tof = "";

            avoid_flag=false;

        //first look for usb(serial) ports

        foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
            if(serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier()){

                if(serialPortInfo.vendorIdentifier() == odrv_2_vendor_id){
                    if(serialPortInfo.productIdentifier() == odrv_2_product_id){
                        if(serialPortInfo.serialNumber() == odrv_2_serial_number){
                            odrv_2_port_name = serialPortInfo.portName();
                            odrv_2_is_available = true;
                        }
                    }
                }

                if(serialPortInfo.vendorIdentifier() == odrv_3_vendor_id){
                    if(serialPortInfo.productIdentifier() == odrv_3_product_id){
                        if(serialPortInfo.serialNumber() == odrv_3_serial_number){
                            odrv_3_port_name = serialPortInfo.portName();
                            odrv_3_is_available = true;
                        }
                    }
                }
/*
                if(serialPortInfo.vendorIdentifier() == stm32_vendor_id){
                                    if(serialPortInfo.productIdentifier() == stm32_product_id){
                                        if(serialPortInfo.description() == description_tof){
                                           tof_name = serialPortInfo.portName();
                                           tof_is_available = true;
                                        }
                                    }
                                 }
  */
      }
    }
        if(odrv_2_is_available){
            // open and configure the serialport
            odrv_2->setPortName(odrv_2_port_name);
            odrv_2->open(QSerialPort::ReadWrite);
            odrv_2->setBaudRate(QSerialPort::Baud115200);
            odrv_2->setParity(QSerialPort::NoParity);
            odrv_2->setStopBits(QSerialPort::OneStop);
            odrv_2->setFlowControl(QSerialPort::NoFlowControl);
                // found QIODevice::ReadWrite on youtube Qt on Raspberry Pi connecting serial to rs-232
            if(odrv_2->isOpen() == true){
           //     qDebug() << "odrv0 is open...";
                connect(odrv_2, SIGNAL(readyRead()), this, SLOT(on_Received_odrv_2_Data()));
            }
        }
        else{
            // give error message if not available
            QMessageBox::warning(this, "Port error", "Couldn't find the odrv_2!");
            }


                if(odrv_3_is_available){
                    // open and configure the serialport
                    odrv_3->setPortName(odrv_3_port_name);
                    odrv_3->open(QSerialPort::ReadWrite);
                    odrv_3->setBaudRate(QSerialPort::Baud115200);
                    odrv_3->setParity(QSerialPort::NoParity);
                    odrv_3->setStopBits(QSerialPort::OneStop);
                    odrv_3->setFlowControl(QSerialPort::NoFlowControl);
                        //
                    if(odrv_3->isOpen() == true){
                   //     qDebug() << "odrv0 is open...";
                        connect(odrv_3, SIGNAL(readyRead()), this, SLOT(on_Received_odrv_3_Data()));
                    }
                }
                else{
                    // give error message if not available
                    QMessageBox::warning(this, "Port error", "Couldn't find the odrv_3!");
                }

/*
            if(tof_is_available){
                        // open and configure the serialport
                        tof->setPortName(tof_name);
                        tof->open(QSerialPort::ReadOnly);    // This is read only stream (sensory input)
                        tof->setBaudRate(QSerialPort::Baud115200);
                        tof->setDataBits(QSerialPort::Data8);
                        tof->setParity(QSerialPort::NoParity);
                        tof->setStopBits(QSerialPort::OneStop);
                        tof->setFlowControl(QSerialPort::NoFlowControl);
                        QObject::connect(tof, SIGNAL(readyRead()), this, SLOT(read_tof()));
                    }
                    else{
                        // give error message if not available
                        QMessageBox::warning(this, "Port error", "Couldn't find tof device!");
                    }
*/
            steer_left = false;
            steer_right = false;
            // set the following so a direction button can be pushed from the start
            // and DD will move
         //   on_speed_doubleSpinBox_valueChanged(.5);
            on_checkBox_stateChanged(1);
            on_idle_pushButton_clicked();
            on_forward_pushButton_released();
}

w4od::~w4od()
{

      if(odrv_2->isOpen())
            odrv_2->close();
      if(odrv_3->isOpen())
            odrv_3->close();

    delete ui;
}


void w4od::on_Received_odrv_2_Data()
{
   // QByteArray read_ba;
    read_ba = odrv_2->readAll();
   ui->textBrowser->insertPlainText(read_ba);
    qDebug() << read_ba;
}

void w4od::on_Received_odrv_3_Data()
{
   // QByteArray read_ba;
    read_ba = odrv_3->readAll();
   ui->textBrowser_3->insertPlainText(read_ba);
    qDebug() << read_ba;
}


void w4od::read_tof()
{

    if(tof->canReadLine())
    {

           serial_tof_data = tof->readAll();
           tof_buffer = QString::fromStdString(serial_tof_data.toStdString());
           tof_buffer.remove("\r\n");
           //qDebug() << tof_buffer;
           serial_tof_data.clear();
        }

              parse_tof();
}




void w4od::parse_tof()
{
    // notes about parse_tof()
    // using qDebug() I found that front had a 0 value twice
    // so a value of zero has to be allowed for in stop avoidance code

    int len;
    int llen;
    int rlen;


    QString left = "l";
    QString right = "r";
    QString cpy = tof_buffer;
    tof_buffer.remove(" ");
    len = tof_buffer.length();
    //qDebug() << tof_buffer;

    if(len <= 27)
    {
        rlen = tof_buffer.indexOf(right);
        front_tof = tof_buffer;
        front_tof.truncate(rlen);
        cpy = tof_buffer.remove(front_tof);
        front_tof.remove("f");
        front = front_tof.toInt();

        ui->lcdNumber_frnt_tof->display(front);
        tof_buffer = cpy;
        len=tof_buffer.length();
        llen = tof_buffer.indexOf(left);
        right_tof = tof_buffer;
        right_tof.truncate(llen);
        cpy = tof_buffer.remove(right_tof);
        right_tof.remove(right);
        fstarboard = right_tof.toInt();
        ui->lcdNumber_strb_tof->display(fstarboard);
        left_tof = cpy;
        left_tof.remove(left);
        fport = left_tof.toInt();
        ui->lcdNumber_port_tof->display(fport);

        // stop avoidance code
        //========================front avoidance and unjam ==================
         if(avoid_flag==true && front!=0 && front<ui->spinBox_frnt_stop->value())
         {
             if(ui->auto_un_jam_checkBox->isChecked()==true)
             {
                 on_unjam_pushButton_clicked();
             }

             else on_forward_pushButton_released();
          }

        //==================port(left) side avoidance and unjam===================

         if(avoid_flag==true && fport!=00 && fport<ui->spinBox_port_stop->value())
         {
             //on_forward_pushButton_released();
             on_unjam_pushButton_clicked();
         }
/*
         if(avoid_flag==true && fport!= 0 && fport<(ui->spinBox_port_stop->value()+100) && fport>ui->spinBox_port_stop->value())
         {
             on_left_pushButton_pressed();

         }

         if(avoid_flag==true && fport!= 0 && fport>(ui->spinBox_port_stop->value()+100) && fport>ui->spinBox_port_stop->value())
          {
             on_left_pushButton_released();

         }
*/
         //====================starboard(right) side avoidance and unjam========================

         if(avoid_flag==true && fstarboard!=0 && fstarboard<ui->spinBox_strb_stop->value())
          {
              //on_forward_pushButton_released();
              on_unjam_pushButton_clicked();
          }
/*
          if(avoid_flag==true && fstarboard!= 0 && fstarboard<(ui->spinBox_strb_stop->value()+100) && fport>ui->spinBox_strb_stop->value())
          {
               on_right_pushButton_pressed();

          }

          if(avoid_flag==true && fstarboard != 0 && fstarboard>(ui->spinBox_strb_stop->value()+100) && fstarboard>ui->spinBox_strb_stop->value())
           {
              on_right_pushButton_released();

          }
    */
   }
}


void w4od::on_forward_pushButton_pressed()
{
    if(ui->avoidance_checkBox->isChecked()==true)
        avoid_flag=true;
    str_velocity=QString::number(velocity_bu);

    // check to see if front wheel drive is engaged
    if(ui->avoidance_checkBox->isChecked() == true)
    {
          // qDebug() <<  front;
        ui->textBrowser->insertPlainText(QString::number(front)+"/n");

    }

    if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }
    if(odrv_2->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }

}

void w4od::on_left_pushButton_pressed()
{
    avoid_flag=false;
    left_pressed();
}

void w4od::left_pressed()
{


    float left_wheel = velocity_bu/steer_divisor;
    str_velocity_0 = QString::number(left_wheel);
    // check to see if front wheel drive is engaged
    str_velocity=QString::number(velocity_bu);

    //str_velocity_0 = "0.00";
    str_velocity_1 = str_velocity;

    if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }

    //motor0 = "v 0 ";
    //motor1 = "v 1 ";
    if(odrv_2->isOpen() == true)
    {
        motor0.append(str_velocity_1);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity_0);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {
        motor0.append(str_velocity_1);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity_0);
        motor1.append("\n");

        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }

 /*   on_avoidance_checkBox_clicked(true);
    if(avoid_flag==true && fport!= 0 && fport>(ui->spinBox_port_stop->value()+100) && fport>ui->spinBox_port_stop->value())
        on_left_pushButton_released();
*/
}

void w4od::on_right_pushButton_pressed()
{
    avoid_flag=false;
    right_pressed();
}
void w4od::right_pressed()
{

    float right_wheel = velocity_bu/steer_divisor;
    str_velocity_1 = QString::number(right_wheel);

    str_velocity=QString::number(velocity_bu);

    //str_velocity_0 = "0.00";
    str_velocity_0 = str_velocity;
    // check to see if front wheel drive is engaged
    if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }

    // motor0 = "v 0 ";
    // motor1 = "v 1 ";
    if(odrv_2->isOpen() == true)
    {
        motor0.append(str_velocity_1);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity_0);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {
        motor0.append(str_velocity_1);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity_0);
        motor1.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }

/*
    on_avoidance_checkBox_clicked(true);
    if(avoid_flag==true && fstarboard!= 0 && fstarboard>(ui->spinBox_strb_stop->value()+100) && fstarboard>ui->spinBox_strb_stop->value())
        on_right_pushButton_released();
*/
}

void w4od::on_forward_pushButton_released()
{
    avoid_flag=false;
    forward_released();
}

void w4od::forward_released()
{

    str_velocity = "0.00";
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    // check to see if front wheel drive is engaged
    if(odrv_2->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }

}

void w4od::on_reverse_pushButton_pressed()
{
    avoid_flag=false;  //
    str_velocity=QString::number(velocity_bu);
    // check to see if front wheel drive is engaged
    if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }
  // motor0 = "v 0 ";
  // motor1 = "v 1 ";
    if(odrv_2->isOpen() == true)
    {
        motor1.append(str_velocity);
        motor1.append("\n");
        motor0.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor0.append(str_velocity);
        motor0.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {
        motor1.append(str_velocity);
        motor1.append("\n");
        motor0.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor0.append(str_velocity);
        motor0.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }

    qDebug() << "reverse";
}

void w4od::on_reverse_pushButton_released()
{
    avoid_flag=false;
    reverse_released();
}

void w4od::reverse_released()
{
    on_avoidance_checkBox_clicked(true);
    str_velocity = "0.00";
    // check to see if front wheel drive is engaged
    if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }
    // motor0 = "v 0 ";
    // motor1 = "v 1 ";
    if(odrv_2->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }


    if(front_unjam==true && unjam_cnt==2)
    {
        unjam_cnt=1;
        on_unjam_pushButton_clicked();
    }

}

void w4od::on_left_pushButton_released()
{
    avoid_flag=false;
    left_released();
}

void w4od::left_released()
{
    str_velocity = "0.00";
    // check to see if front wheel drive is engaged
    if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }

   // motor0 = "v 0 ";
   // motor1 = "v 1 ";
    if(odrv_2->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }



    if(ui->speed_constant_checkBox->isChecked()==true)
       on_forward_pushButton_pressed();
}


void w4od::on_right_pushButton_released()
{
    avoid_flag=false;
    right_released();
}

void w4od::right_released()
{
    str_pvt_spd = "0.00";
    // check to see if front wheel drive is engaged
    if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }
  //  motor0 = "v 0 ";
  //  motor1 = "v 1 ";
    if(odrv_2->isOpen() == true)
    {
        motor0.append(str_pvt_spd);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_pvt_spd);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }

     if(ui->speed_constant_checkBox->isChecked()==true)
        on_forward_pushButton_pressed();
}

void w4od::on_pivot_starboard_pushButton_pressed()
{
    str_pvt_spd=QString::number(pvt_spd);
    // check to see if front wheel drive is engaged
  /*  if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }
    */
     motor0 = "v 0 ";
     motor1 = "v 1 ";
    if(odrv_2->isOpen() == true)
    {

            motor0.append("-");     //negative is needed to make motor1 go same direction as motor0

        motor0.append(str_pvt_spd);
        motor0.append("\n");


        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0

        motor1.append(str_pvt_spd);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {

            motor0.append("-");     //negative is needed to make motor1 go same direction as motor0

        motor0.append(str_pvt_spd);
        motor0.append("\n");


        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0

        motor1.append(str_pvt_spd);
        motor1.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }
//    qDebug() << "starboard";
}

void w4od::on_pivot_port_pushButton_pressed()
{
    str_pvt_spd=QString::number(pvt_spd);
  /*  // check to see if front wheel drive is engaged
    if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }
    */
        motor0 = "v 0 ";
        motor1 = "v 1 ";

    if(odrv_3->isOpen() == true)
    {
       /* if(front_wd_flag==true)
        {
            motor0.append("-");     //negative is needed to make motor1 go same direction as motor0
        }*/
        motor0.append(str_pvt_spd);
        motor0.append("\n");
       /* if(front_wd_flag==true)
        {
            motor0.append("-");     //negative is needed to make motor1 go same direction as motor0
        }*/
        motor1.append(str_pvt_spd);
        motor1.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }

    if(odrv_2->isOpen() == true)
    {
       /* if(front_wd_flag==true)
        {
            motor0.append("-");     //negative is needed to make motor1 go same direction as motor0
        }*/
        motor0.append(str_pvt_spd);
        motor0.append("\n");
       /* if(front_wd_flag==true)
        {
            motor0.append("-");     //negative is needed to make motor1 go same direction as motor0
        }*/
        motor1.append(str_pvt_spd);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

  //  qDebug() << "port";
}

void w4od::on_pivot_port_pushButton_released()
{
    str_pvt_spd = "0.00";
    // check to see if front wheel drive is engaged
    if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }
    // motor0 = "v 0 ";
    // motor1 = "v 1 ";
    if(odrv_2->isOpen() == true)
    {
        motor0.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor0.append(str_pvt_spd);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_pvt_spd);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }


    if(front_unjam == true && unjam_cnt==1)
    {
        front_unjam=false;
        unjam_cnt=0;
        if(ui->speed_constant_checkBox->isChecked()==true)
            on_forward_pushButton_pressed();
    }

}

void w4od::on_pivot_starboard_pushButton_released()
{
    str_pvt_spd = "0.00";
    // check to see if front wheel drive is engaged
    if(front_wd_flag==true)
    {
        motor0 = "v 1 ";
        motor1 = "v 0 ";

    }
    else{
    motor0 = "v 0 ";
    motor1 = "v 1 ";
    }
     //    motor0 = "v 0 ";
     //    motor1 = "v 1 ";
    if(odrv_2->isOpen() == true)
    {
        motor0.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor0.append(str_pvt_spd);

        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_pvt_spd);
        motor1.append("\n");
        odrv_2->write(motor0);
        odrv_2->write(motor1);
    }

    if(odrv_3->isOpen() == true)
    {
        motor0.append(str_velocity);
        motor0.append("\n");
        motor1.append("-");     //negative is needed to make motor1 go same direction as motor0
        motor1.append(str_velocity);
        motor1.append("\n");
        odrv_3->write(motor0);
        odrv_3->write(motor1);
    }

}


void w4od::on_show_volt_pushButton_clicked()
{
    if(odrv_3->isOpen()==true){
        ui->textBrowser_3->clear();
        ui->textBrowser_3->insertPlainText("Battery Voltage -- ");
        odrv_3->write("r vbus_voltage\r");
        //read_ba = odrv_3->readAll();
        }
    if(odrv_2->isOpen()==true){
        ui->textBrowser->clear();
        ui->textBrowser->insertPlainText("Battery Voltage -- ");
        odrv_2->write("r vbus_voltage\r");
        //read_ba = odrv_2->readAll();
        }

}

void w4od::on_closed_loop_pushButton_clicked()
{
    if(ui->odrv_2_ck->isChecked() == true && odrv_2->isOpen()==true)
    {
        odrv_2->write("w axis0.requested_state 8\n");
        odrv_2->write("w axis1.requested_state 8\n");

        ui->label_odrv2_ei->clear();
        ui->label_odrv2_ei->setText("Engage");
    }

    if(ui->odrv_3_ck->isChecked() == true && odrv_3->isOpen()==true)
    {
        odrv_3->write("w axis0.requested_state 8\n");
        odrv_3->write("w axis1.requested_state 8\n");

        ui->label_odrv3_ei->clear();
        ui->label_odrv3_ei->setText("Engage");
    }

}


void w4od::on_idle_pushButton_clicked()
{
    if(ui->odrv_2_ck->isChecked() == true && odrv_2->isOpen()==true)
    {
        odrv_2->write("w axis0.requested_state 1\n");
        odrv_2->write("w axis1.requested_state 1\n");

        ui->label_odrv2_ei->clear();
        ui->label_odrv2_ei->setText("Idle");
    }

    if(ui->odrv_3_ck->isChecked() == true && odrv_3->isOpen()==true)
    {
        odrv_3->write("w axis0.requested_state 1\n");
        odrv_3->write("w axis1.requested_state 1\n");

        ui->label_odrv3_ei->clear();
        ui->label_odrv3_ei->setText("Idle");
    }
}

void w4od::on_cal_encoder_pushButton_clicked()
{
   // modified for check boxes

    if(ui->odrv_2_ck->isChecked() ==true && odrv_2->isOpen()==true)
    {
        odrv_2->write("w axis0.encoder.config.calib_range = 0.05\n");
        odrv_2->write("w axis0.requested_state 7\n");
        odrv_2->write("w axis1.encoder.config.calib_range = 0.05\n");
        odrv_2->write("w axis1.requested_state 7\n");

    }

    if(ui->odrv_3_ck->isChecked() ==true && odrv_3->isOpen()==true)
    {
        odrv_3->write("w axis0.encoder.config.calib_range = 0.05\n");
        odrv_3->write("w axis0.requested_state 7\n");
        odrv_3->write("w axis1.encoder.config.calib_range = 0.05\n");
        odrv_3->write("w axis1.requested_state 7\n");

    }
}





void w4od::on_turn_ratio_doubleSpinBox_valueChanged(double arg1)
{
    steer_divisor = arg1;
}


void w4od::on_speed_doubleSpinBox_valueChanged(double arg1)
{
    velocity_bu = arg1;
    if(ui->speed_constant_checkBox->isChecked()==true)
        on_forward_pushButton_pressed();
    qDebug() << velocity_bu;
}

void w4od::on_pivot_spd_doubleSpinBox_valueChanged(double arg1)
{
    pvt_spd =arg1;

}

void w4od::on_checkBox_stateChanged(int arg1)
{
    if(arg1!=0)
    {
        front_wd_flag = true;
    }
    else front_wd_flag = false;
}

void w4od::on_sys_info_pushButton_clicked()
{
        first_info();
        second_info();
}


void w4od::first_info()
{

    ui->textBrowser->clear();
    ui->textBrowser_3->clear();

    if(odrv_2->isOpen()==true){
        ui->textBrowser->insertPlainText("Wheel0 ");
        odrv_2->write("r axis0.encoder.pos_estimate\r");
        }
    if(odrv_3->isOpen()==true){
        ui->textBrowser_3->insertPlainText("Wheel0 ");
        odrv_3->write("r axis0.encoder.pos_estimate\r");
        }


 }

 void w4od::second_info()
{
    if(odrv_2->isOpen()==true){
        ui->textBrowser->insertPlainText("Wheel1 ");
        odrv_2->write("r axis1.encoder.pos_estimate\r");
        }
    if(odrv_3->isOpen()==true){
        ui->textBrowser_3->insertPlainText("Wheel1 ");
        odrv_3->write("r axis1.encoder.pos_estimate\r");
        }


 }





void w4od::on_speed_constant_checkBox_clicked(bool checked)
{
    if(checked==false)
        on_forward_pushButton_released();

}

void w4od::on_avoidance_checkBox_clicked(bool checked)
{
    if(checked==true)
        avoid_flag=true;
    else avoid_flag=false;
}

void w4od::on_unjam_pushButton_clicked()
{
    if(unjam_cnt==0 && front_unjam==false)
    {
        unjam_cnt=2;
        front_unjam=true;
        on_reverse_pushButton_pressed();
        timed_reverse_release();
    }
    if(unjam_cnt==1 && front_unjam==true)
    {
        if(fport>fstarboard)
        {
        on_pivot_port_pushButton_pressed();
        timed_uturn();
        }

        if(fport<fstarboard)
        {
            on_pivot_starboard_pushButton_pressed();
            timed_uturn();
        }
    }

}

void w4od::timed_reverse_release()
{
    QTimer *timer =new QTimer();
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(on_reverse_pushButton_released()));
    timer->setSingleShot(true);
    timer->start(ui->timer_spinBox->value());

}

void w4od::timed_uturn()
{
    if(fport>fstarboard){
    QTimer *timer =new QTimer();
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(on_pivot_port_pushButton_released()));
    timer->setSingleShot(true);
    timer->start(ui->timer_spinBox->value());
    }

   else if(fport<fstarboard){
    QTimer *timer =new QTimer();
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(on_pivot_starboard_pushButton_released()));
    timer->setSingleShot(true);
    timer->start(ui->timer_spinBox->value());
    }

}



void w4od::on_reboot_odrv_pushButton_clicked()
{

}
