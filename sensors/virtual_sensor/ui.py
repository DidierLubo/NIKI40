# encoding: utf-8

# Copyright 2017 NIKI 4.0 project team
#
# NIKI 4.0 was financed by the Baden-Württemberg Stiftung gGmbH (www.bwstiftung.de).
# Project partners are FZI Forschungszentrum Informatik am Karlsruher
# Institut für Technologie (www.fzi.de), Hahn-Schickard-Gesellschaft
# für angewandte Forschung e.V. (www.hahn-schickard.de) and
# Hochschule Offenburg (www.hs-offenburg.de).
# This file was developed by Mark Weyer at Hahn-Schickard.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""The Qt interface to the virtual sensor."""

import PySide.QtCore as core
import PySide.QtGui as gui

import deamon
import waveform as wf
import resource as res


class Internal(Exception): pass


class window(gui.QWidget):
    """The ui window."""

    def __init__(self,config,resources,timer):

        self.resources = resources
        self.config = config
        self.error_label = gui.QLabel()
        self.started = False

        # We need to prevent some things from being garbage collected.
        self.keep_alive = []

        gui.QWidget.__init__(self)
        self.setWindowTitle("Virtual sensor remote control")

        def line_edit(key, of_string = lambda x: x, to_string = lambda x: x):
            widget = gui.QLineEdit()
            config.connect(key,
                lambda handle: widget.editingFinished.connect(
                    lambda: handle(of_string(widget.text()))),
                lambda v: widget.setText(to_string(v)))
            return widget

        main_layout = gui.QVBoxLayout(self)
        self.keep_alive.append(main_layout)
        logo = gui.QLabel()
        logo.setPixmap(gui.QPixmap("logo.png"))
        main_layout.addWidget(logo)
        main_layout.setAlignment(logo,core.Qt.AlignRight)

        # It is time for a first separator line
        hline = gui.QFrame()
        main_layout.addWidget(hline)
        hline.setFrameShape(gui.QFrame.HLine)
        hline.setFrameShadow(gui.QFrame.Plain)

        # The sensor definition file name
        sens_def_layout = gui.QHBoxLayout()
        self.keep_alive.append(sens_def_layout)
        main_layout.addLayout(sens_def_layout)
        sens_def_form_layout = gui.QFormLayout()
        self.keep_alive.append(sens_def_form_layout)
        sens_def_layout.addLayout(sens_def_form_layout)
        self.sens_def_filename = line_edit('sensor_definition_filename')
        sens_def_form_layout.addRow("Name of sensor definition file",
            self.sens_def_filename)
        sens_def_browse_button = gui.QPushButton("Browse")
        sens_def_layout.addWidget(sens_def_browse_button)
        sens_def_browse_button.clicked.connect(self.browse_sens_def)
        self.last_sens_def_filename = None

        # Now comes the part pertaining to resource definition
        self.outer_resource_widget = gui.QStackedWidget()
        main_layout.addWidget(self.outer_resource_widget)
        self.inner_resource_widget = None
        self.sens_def_filename.editingFinished.connect(self.init_resources)
        self.sens_def_filename.editingFinished.connect(lambda: None)
        # later we will call self.init_resources, but only after all its
        # prerequisites are there

        # Next we have global settings, but first another separator line
        hline = gui.QFrame()
        main_layout.addWidget(hline)
        hline.setFrameShape(gui.QFrame.HLine)
        hline.setFrameShadow(gui.QFrame.Plain)

        common_spec_layout = gui.QFormLayout()
        self.keep_alive.append(common_spec_layout)
        main_layout.addLayout(common_spec_layout)
        port = line_edit('port_name')
        common_spec_layout.addRow("Serial port",port)
        ip = line_edit('host_ip')
        ip.setInputMask('hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh')
        common_spec_layout.addRow("Server IP",ip)
        host_port = line_edit('host_port', lambda s: int(s), lambda i: str(i))
        host_port.setValidator(gui.QIntValidator(0,65535))
        common_spec_layout.addRow("Server port",host_port)
        channel = line_edit('slip_channel', lambda s: int(s), lambda i: str(i))
        channel.setValidator(gui.QIntValidator(0,255))
        common_spec_layout.addRow("Channel",channel)
        mac = line_edit('client_mac')
        mac.setInputMask('hh:hh:hh:hh:hh:hh:hh:hh')
        common_spec_layout.addRow("Client MAC",mac)
        client = line_edit('client_name')
        client.setMaxLength(32)
        common_spec_layout.addRow("Client name",client)
        delay = gui.QDoubleSpinBox()
        delay.setRange(0.001,100)
        config.connect('update_delay',
            delay.valueChanged.connect, delay.setValue)
        common_spec_layout.addRow("Delay between updates / s",delay)

        # Configuration buttons
        button_layout = gui.QHBoxLayout()
        self.keep_alive.append(button_layout)
        main_layout.addLayout(button_layout)
        load_button = gui.QPushButton("Load defaults")
        button_layout.addWidget(load_button)
        load_button.clicked.connect(config.load)
        save_button = gui.QPushButton("Save as defaults")
        button_layout.addWidget(save_button)
        save_button.clicked.connect(config.save)

        # Another separator line
        hline = gui.QFrame()
        main_layout.addWidget(hline)
        hline.setFrameShape(gui.QFrame.HLine)
        hline.setFrameShadow(gui.QFrame.Plain)

        # Status
        progress = gui.QProgressBar()
        main_layout.addWidget(progress)
        status = gui.QLabel()
        main_layout.addWidget(status)
        main_layout.addWidget(self.error_label)
        self.error_palette = self.error_label.palette()
        self.error_palette.setColor(gui.QPalette.WindowText,gui.QColor(184,0,0))
        self.error_label.setPalette(self.error_palette)

        # Control Buttons
        button_layout = gui.QHBoxLayout()
        self.keep_alive.append(button_layout)
        main_layout.addLayout(button_layout)
        self.start_stop_button = gui.QPushButton("Start")
        button_layout.addWidget(self.start_stop_button)
        quit_button = gui.QPushButton("Quit")
        quit_button.clicked.connect(self.close)
        button_layout.addWidget(quit_button)

        # Finally we connect the functionality
        self.disable_widgets = \
            [sens_def_browse_button, load_button]
        self.lock_widgets = \
            [self.sens_def_filename, port, ip, host_port, channel, mac, client]
        self.deamon = deamon.Deamon(self, config, progress, status, resources)
        self.init_resources()
        self.start_stop_button.clicked.connect(self.start_stop)
        timer.timeout.connect(self.deamon.tick)
        timer.start()

        self.show()


    def start_stop(self):
        if self.started:
            self.deamon.stop()
            self.started = False
            self.start_stop_button.setText("Start")
        else:
            self.deamon.start()
            self.started = True
            self.start_stop_button.setText("Stop")

    def init_resources(self):
        new_sens_def_filename = self.config.value('sensor_definition_filename')
        if new_sens_def_filename == self.last_sens_def_filename:
            return

        self.last_sens_def_filename = new_sens_def_filename
        try:
            self.resources.load(new_sens_def_filename)
            self.error("")
        except res.SensorDefinitionFileError:
            self.error("Could not load file " + new_sens_def_filename)
        except res.SensorDefinitionParseError:
            self.error("Could not parse file " + new_sens_def_filename)
        except res.SensorDefinitionTypeError:
            self.error("Type error in file " + new_sens_def_filename)

        self.config.init_resources(self.resources)

        num_resources = 0
        num_fields = [0]
            # The list will remain of length 1. It serves as a wrapper around
            # a single mutable field. We need some kind of wrapper to avoid
            # scoping problems when assigning inside of field_column.
        field_columns = {}

        if self.inner_resource_widget is not None:
            self.outer_resource_widget.removeWidget(self.inner_resource_widget)
            self.inner_resource_widget.deleteLater()
        inner_inner_widget = gui.QWidget()
        layout = gui.QGridLayout(inner_inner_widget)

        def field_column(name):
            if name in field_columns:
                return field_columns[name]
            else:
                num_fields[0] += 1
                field_columns[name] = num_fields[0]
                layout.addWidget(gui.QLabel(name), 0, num_fields[0])
                return num_fields[0]

        for resource in self.resources.list():
            if resource.ui_fields != []:
                num_resources += 1
                layout.addWidget(gui.QLabel(resource.ui_name), num_resources, 0)

                def handle_float_field(field):
                    widget = gui.QDoubleSpinBox()
                    widget.setRange(
                        field.extra_data.range_min, field.extra_data.range_max)
                    self.config.connect_resource(
                        resource.config_name, field.config_name,
                        widget.valueChanged.connect, widget.setValue)
                    layout.addWidget(
                        widget, num_resources, field_column(field.ui_name))

                def handle_enum_field(field):
                    widget = gui.QComboBox()
                    enum = field.extra_data.enum
                    widget.addItems([key for key,form in enum.list])
                    self.config.connect_resource(
                        resource.config_name, field.config_name,
                        lambda slot:
                            widget.currentIndexChanged.connect(
                                lambda index:
                                    slot(enum.list[index][0])),
                        lambda name:
                            widget.setCurrentIndex(enum.index_map[name]))
                    layout.addWidget(
                        widget, num_resources, field_column(field.ui_name))
                
                def handle_string_field(field):
                    widget = gui.QLineEdit()
                    widget.setMaxLength(field.extra_data.max_length)
                    self.config.connect_resource(
                        resource.config_name, field.config_name,
                        lambda slot:
                            widget.editingFinished.connect(
                                lambda: slot(widget.text())),
                        lambda v: widget.setText(v))
                    layout.addWidget(
                        widget, num_resources, field_column(field.ui_name))

                for field in resource.ui_fields:
                    if field.type == res.ui_type_float:
                        handle_float_field(field)
                    elif field.type == res.ui_type_enum:
                        handle_enum_field(field)
                    elif field.type == res.ui_type_string:
                        handle_string_field(field)
                    else:
                        raise Internal()

        if num_resources <= 6:
            layout.setContentsMargins(0,0,0,0)
            self.inner_resource_widget = inner_inner_widget
        else:
            self.inner_resource_widget = gui.QScrollArea()
            self.inner_resource_widget.setWidget(inner_inner_widget)
            self.inner_resource_widget.setMinimumWidth(
                inner_inner_widget.sizeHint().width()
                + layout.contentsMargins().right()
                + self.inner_resource_widget.verticalScrollBar()
                    .sizeHint().width())
            self.inner_resource_widget.setMinimumHeight(
                self.size().height() // 2)
        self.outer_resource_widget.addWidget(self.inner_resource_widget)
        self.updateGeometry()
        self.adjustSize()
        self.repaint()
        self.deamon.init_measurements()


    def lock_unlock(self,lock):
        """Lock or unlock those widgets which cannot be changed during
        operation."""
        for widget in self.disable_widgets:
            widget.setDisabled(lock)
        for widget in self.lock_widgets:
            widget.setReadOnly(lock)


    def error(self,message):
        """Update the displayed error message."""
        self.error_label.setText(message)


    def browse_sens_def(self):
        filename,_ = gui.QFileDialog.getOpenFileName(
            self,
            "Load sensor definition",
            "",
            "JSON files (*.json)")
        if filename != "":
            self.sens_def_filename.setText(filename)
            self.sens_def_filename.editingFinished.emit()

