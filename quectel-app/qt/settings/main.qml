import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.3

import About 1.0

ApplicationWindow {
    visible: true
    title: qsTr("Settings")

    About {
        id: about_id
    }

    Button {
        width: 120
        height: 80
        text: qsTr("Exit")
        anchors {bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; bottomMargin: 20 }
        background:Rectangle{
            anchors.fill: parent
            color: "red"
        }

        onClicked: {
            Qt.quit()
        }
    }

    Column {
        spacing: 4

        Rectangle{//模拟线段
            width: 800
            height:80  //高
        }

        Text {
            font.pixelSize: 40
            text: "Settings and BoardInfos"
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            color:"lightgray" //颜色
        }

        Text {
            id: kernelVersion_id
            font.pixelSize: 30
            text: about_id.kernelVersion
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            visible: (about_id.kernelVersion.length == 0)? false:true
            color:"lightgray" //颜色
        }

        Text {
            id: gccVersion_id
            font.pixelSize: 30
            text: about_id.gccVersion
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            color:"lightgray" //颜色
        }

        Text {
            id: compileTime_id
            font.pixelSize: 30
            text: about_id.compileTime
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            color:"lightgray" //颜色
        }

        Text {
            id: bpVersion_id
            font.pixelSize: 30
            text: about_id.bpVersion
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            visible: (about_id.bpVersion.length == 0)? false:true
            color:"lightgray" //颜色
        }

        Text {
            id: imei_id
            font.pixelSize: 30
            text: about_id.IMEI
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            visible: (about_id.IMEI.length == 0)? false:true
            color:"lightgray" //颜色
        }

        Text {
            id: imei2_id
            font.pixelSize: 30
            text: about_id.IMEI2
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            visible: (about_id.IMEI2.length == 0)? false:true
            color:"lightgray" //颜色
        }

        Text {
            id: meid_id
            font.pixelSize: 30
            text: about_id.MEID
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            visible: (about_id.MEID.length == 0)? false:true
            color:"lightgray" //颜色
        }

        Text {
            id: wifimac_id
            font.pixelSize: 30
            text: about_id.WIFIMAC
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            visible: (about_id.WIFIMAC.length == 0)? false:true
            color:"lightgray" //颜色
        }

        Text {
            id: btmac_id
            font.pixelSize: 30
            text: about_id.BTMAC
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            visible: (about_id.BTMAC.length == 0)? false:true
            color:"lightgray" //颜色
        }

        Text {
            id: memTotal_id
            font.pixelSize: 30
            text: about_id.memTotal
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            color:"lightgray" //颜色
        }

        Text {
            id: memAvailable_id
            font.pixelSize: 30
            text: about_id.memAvailable
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            color:"lightgray" //颜色
        }

        Text {
            id: flashSize_id
            font.pixelSize: 30
            text: about_id.flashSize
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            color:"lightgray" //颜色
        }

        Text {
            id: sdTotalSize_id
            font.pixelSize: 30
            visible: (about_id.sdTotalSize.length == 0) ? false:true
            text: about_id.sdTotalSize
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            visible: (about_id.sdTotalSize.length == 0) ? false:true
            color:"lightgray" //颜色
        }

        Text {
            id: sdAvailableSize_id
            font.pixelSize: 30
            visible: (about_id.sdAvailableSize.length == 0) ? false:true
            text: about_id.sdAvailableSize
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            visible: (about_id.sdAvailableSize.length == 0) ? false:true
            color:"lightgray" //颜色
        }

        Text {
            id: powerStatus_id
            font.pixelSize: 30
            text: about_id.powerStatus
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            color:"lightgray" //颜色
        }

        Text {
            id: batteryCapacity_id
            font.pixelSize: 30
            text: about_id.batteryCapacity
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            color:"lightgray" //颜色
        }

        Text {
            id: rtcTime_id
            font.pixelSize: 30
            text: about_id.rtcTime
        }

        Rectangle{//模拟线段
            width: 800
            height:1  //高
            visible: (about_id.rtcTime.length == 0) ? false:true
            color:"lightgray" //颜色
        }

        RowLayout {
            spacing: 40
            Text {
                id: brightness_id
                font.pixelSize: 30
                visible: (about_id.brightness < 0 || about_id.brightness > 255) ? false:true
                text: "Brightness:"
            }

            Slider {
                width: 255
                visible: (about_id.brightness < 0 || about_id.brightness > 255) ? false:true
                from: 0
                to: 255
                value: about_id.brightness+10
                stepSize:5
                onValueChanged: {
                    about_id.brightness = value
                }
            }
        }
    }
}
