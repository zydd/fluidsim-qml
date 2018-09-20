import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import zydd.fluid 1.0

Window {
    id: root
    width: 640*1366/768
    height: 640
    visible: true

    FluidSim {
        id: sim
//            width: height; height: Math.min(root.width,root.height)
        anchors.fill: parent
        g: g.value
        k: k.value
        v: v.value
        vconf: vconf.value
        dt: 0.001+0.999*Math.pow(dt.value,2)
        factor: Math.round(1+99*Math.pow(factor.value,2))
        simw: 256*1366/768
        simh: 256
        running: true
        focus: true
        display: 0

        property int button: 0

        onFocusChanged: focus = true

        MouseArea {
            id: ma
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onDoubleClicked: sim.reset()
            onPressed: {
                sim.button = mouse.button
                sim.running = true
            }

            onReleased: {
                if (sim.button === mouse.button)
                    sim.button = 0;
            }

            onPressAndHold: {
                if (mouseX < 10 && mouseY < 10)
                    config.visible = true
            }
        }

        Timer {
            running: ma.pressed
            repeat: true
            interval: 16
            onTriggered: {
                if (stopsim.running)
                    stopsim.restart()
                sim.den_ellipse(Qt.point(ma.mouseX,ma.mouseY),brush.value,
                                sim.button === Qt.RightButton ? 0.5 : -0.5)
            }
        }

        Timer {
            id: stopsim
            running: autopause.checked
            interval: 20000
            onTriggered: sim.running = false
        }

        Keys.onPressed: {
            if (event.text >= '1' && event.text <= '6')
                sim.display = Number.fromLocaleString(Qt.locale("de_DE"), event.text)-1
            switch(event.text) {
            case '\'': config.visible = !config.visible; break;
            }
        }
    }

    Rectangle {
        color: 'white'
        width: cfglayout.width
        height: cfglayout.height
        id: config
        visible: false

        Row {
            id: cfglayout

            Column {
                RowLayout {
                    Slider { id: g; from: -1; to: 1; stepSize: 0.001; value: 0.0250 } // 0.07
                    Label { text: 'g: ' + sim.g.toFixed(4) }
                }

                RowLayout {
                    Slider { id: k; from: 0; to: 50; stepSize: 0.01; value: 3.44 } // 2
                    Label { text: 'k: ' + sim.k.toFixed(4) }
                }

                RowLayout {
                    Slider { id: v; from: 0; to: 10; stepSize: 0.001; value: 0.25 } // 0.05
                    Label { text: 'v: ' + v.value.toFixed(4) }
                }

                RowLayout {
                    Slider { id: dt; from: 0; to: 1; stepSize: 0.001; value: Math.sqrt((0.05-0.001)/0.999) }
                    Label { text: 'dt: ' + sim.dt.toFixed(4) }
                }

                RowLayout {
                    Slider { id: vconf; from: 0; to: 5; stepSize: 0.001; value: 0.688 }
                    Label { text: 'vconf: ' + sim.vconf.toFixed(4) }
                }

                RowLayout {
                    Slider { id: factor; from: 0; to: 1; value: Math.sqrt(70/100) }
                    Label { text: 'factor: ' + sim.factor }
                }

                RowLayout {
                    RadioButton { onPressed: sim.display = 0; checked: true }
                    RadioButton { onPressed: sim.display = 1; }
                    RadioButton { onPressed: sim.display = 2 }
                    RadioButton { onPressed: sim.display = 3 }
                    RadioButton { onPressed: sim.display = 4 }
                    RadioButton { onPressed: sim.display = 5 }
                }
                RowLayout {
                    Slider { id: brush; from: 0; to: 0.1; stepSize: 0.001; value: 0.05 }
                    Label { text: 'brush: ' + brush.value.toFixed(4) }
                }

                RowLayout {
                    CheckBox {
                        id: autopause
                        text: 'Auto pause'
                    }

                    Button {
                        text: 'Hide'
                        onPressed: config.visible = false
                    }
                }
            }
        }
    }

    Canvas {
        visible: false
        focus: false
        width: sim.simw
        height: sim.simh
        id: source

        onPaint: {
            var ctx = getContext("2d");
            ctx.fillStyle = '#ff0000';
            ctx.fillRect(0, 0, width, height);

            ctx.fillStyle ='#ff0040';
            ctx.fillRect(width/3, 4*height/10, width/3, height/10);

            ctx.fillStyle ='#ff4000';
            ctx.fillRect(width/3, 5*height/10, width/3, height/10);

//            for (var i = 0; i < 5; ++i)
//                ctx.ellipse(width/3+i*width/15, 7*height/10, 1.5*height/10, 1.5*height/10).fill();

            source.grabToImage(function(img) {
                sim.setInitTex(img);
                sim.reset();
            })
        }

        Component.onCompleted: requestPaint()
    }
}
