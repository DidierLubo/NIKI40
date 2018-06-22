using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ZedGraph;

namespace LVDT_Auswertung
{
  public struct Polynom3
  {
      // Polynom-Struct zum Speichern der Polynom-Werte (VK 2018-03-22)
      public double a;
      public double b;
      public double c;
      public double d;
  }

  public partial class FrmMain : Form
  {
    NumberStyles tempStyle = NumberStyles.AllowLeadingSign | NumberStyles.AllowDecimalPoint;
    Polynom3[] Polynoms = new Polynom3[6]; // Array mit 4 verschiedenen Polynomen für 4 verschiedene LVDT-Typen (VK 2018-03-22)
    char[] charsToTrim = { '\r', ' ' };
    UInt16 timeout_command = 10000;
    bool rs232_connected = false;
    string rs232_receive_string = "";
//    bool rs232_timeout = false;
    private int anzahl_messwerte = 0;
    private double messdauer = 0;
    private double messintervall = 0;
    private bool messung_laeuft = false;
    List<double> l_zeit = new List<double>();
    List<double> l_data = new List<double>();
    List<double> l_data2 = new List<double>();
    System.Diagnostics.Stopwatch stopwatch_rs232 = new System.Diagnostics.Stopwatch();
    CultureInfo culture = System.Globalization.CultureInfo.CurrentCulture;
    DateTime dt;
    public string[] result_strings = new string[5];
    double abweichung_max = 1.0;
    UInt32 ausreisser_anzahl = 0;
    double lvdt_roh = 0.0;
    //bool offsetGeradeGenullt = false;
    double faktor_weg = 1.0;
    double y_min=-0.2;
    double y_max = 0.2;
    double y2_min = -0.2;
    double y2_max = 0.2;
    double offset = 0.0;
    double zeitabschnitt = 0.1;
    public string messdatei = "";
    private bool messdatei_schreiben = false;
    StreamWriter strFile;
    TextWriterTraceListener tl_error_log = new TextWriterTraceListener(@".\ErrorLog.txt");
    public int result_mask = 0;
    public string[] config_reg = new string[7];
    private bool gp22_ready = false;
    double gp22_clock = Properties.Settings.Default.hs_clock;
    UInt32 dds = Properties.Settings.Default.dds_frequency;
    string config_file = Properties.Settings.Default.lvdt_config_file;
    UInt32 gp22_clock_calibration_value = 0;
    UInt32 dds_enable = Properties.Settings.Default.dds_enable;
    int anzahl_mittelwert = 0;
    GraphPane gp;

    RollingPointPairList rppl_lvdt = new RollingPointPairList(1000);
    LineItem li_lvdt;
    LineItem li_temp;

    public bool first_meas = false;
    string mcu_version = "";

    public FrmMain()
    {
      InitializeComponent();
    }

    public void PresetPolynoms()
    {
            // Vorbelegung der Polynome mit Werten aus Matlab (VK 2018-03-22)
            // 0 = 200mm

            Polynoms[0].a = 2.942195736827930e+06;
            Polynoms[0].b = -4.367784223888253e+04;
            Polynoms[0].c = 3.848420976388827e+03;
            Polynoms[0].d = 0;
            /*Polynoms[0].a = -2.109806151431080e+06;
            Polynoms[0].b = 1.932872456172494e+05;
            Polynoms[0].c = -9.686301810232353e+03;
            Polynoms[0].d = 0;*/ //1.664944934952484e+02;
                                 //Polynoms[0].d = 1;
                                 // 1 = 150mm
            Polynoms[1].a = -5.970084428921792e+05;
            Polynoms[1].b = 6.193309675715068e+04;
            Polynoms[1].c = -4.251227353992234e+03;
            Polynoms[1].d = 0; //8.829393277835010e+01;
            // 2 = 100mm
            Polynoms[2].a = -8.508271764651866e+04;
            Polynoms[2].b = 1.059945486741356e+04;
            Polynoms[2].c = -1.302674425718524e+03;
            Polynoms[2].d = 0; //3.585635507186540e+01;
            // 3 = 50mm
            Polynoms[3].a = -1.648652002234710e+04;
            Polynoms[3].b = 1.334785705905020e+03;
            Polynoms[3].c = -1.814201486181140e+02;
            Polynoms[3].d = 0; //3.067818515005505e+00;
            // 5 = 50Si
            Polynoms[4].a = -3.214134090850980e+04;
            Polynoms[4].b = 2.880607890703981e+03;
            Polynoms[4].c = -2.738959017446818e+02;
            Polynoms[4].d = 0; //3.786492917823749e+00;
            // 5 = 50So
            Polynoms[5].a = -2.697961337244921e+04;
            Polynoms[5].b = 2.865650505187431e+03;
            Polynoms[5].c = -2.995406242759050e+02;
            Polynoms[5].d = 0; //7.522185555947591e+00;
        }

    public void error_log(string text)
    {
        string s = "";

        s = DateTime.Now.ToString() + " " + text;
        Trace.WriteLine(s);
    }

    private bool konfigurationsdatei_laden()
    {
        string zeile = "";
        int i = 0;
        bool ret = false;
        bool file_exists = false;

        if (!File.Exists(config_file))
        {
            MessageBox.Show("Konfigurationsdatei GP22_LVDT.cf22 nicht gefunden !");
            return false;
        }

        try
        {
            using (StreamReader strFile = new StreamReader(config_file))
            {
                i = 0;
                while ((zeile = strFile.ReadLine()) != null)
                {
                    config_reg[i] = zeile;
                    i++;
                }
            }

            if (gp22_set_config())
            {
            }
            else
            {
            }
        }
        catch (Exception ex)
        {
            MessageBox.Show(ex.Message);
            ret = false;
        }
        ret = true;

        return ret;
    }


    void messdatei_auswaehlen()
    {
        if (saveFileDialog1.ShowDialog() != DialogResult.OK)
            return;

        messdatei = saveFileDialog1.FileName;

    }

    private void RefreshRS232Ports()
    {
      cbRS232.Items.Clear();
      foreach (string s in SerialPort.GetPortNames())  // Führt die angegebene Aktion(COM-PortName) für jedes Element der List<s> aus
      cbRS232.Items.Add(s);
    }

    private string rs232_get_answer(int timeout)
    {
      string antwort = "";

      serialPort1.ReadTimeout = timeout;
      try
      {
        antwort = serialPort1.ReadLine();
      }
      catch (Exception)
      {
        antwort = "Timeout";
      }

//      tbStatus.Text = antwort;
      return antwort;
    }

    private string get_firmware_version()
    {
        string ret;
        int pos;

        if (!rs232_connected)
            return "";

        serialPort1.Write("ver\r");

        rs232_receive_string = rs232_get_answer(timeout_command);

        if (rs232_receive_string == "Timeout")
        {
            ret = "Timeout";
        }
        else
        {
            pos = rs232_receive_string.IndexOf('=');
            ret = rs232_receive_string.Substring(pos + 1, rs232_receive_string.Length - pos - 1).TrimEnd();
        }

        return ret;
    }

    private bool gp22_get_mess_mode(out int value, out string ret_string)
    {
        string s;
        int pos;
        bool ret = false;

        s = "mess_mode\r";
        serialPort1.Write(s);

        rs232_receive_string = rs232_get_answer(timeout_command);
        if (rs232_receive_string == "Timeout")
        {
            value = -1;
            ret_string = "Timeout";
            return false;
        }
        pos = rs232_receive_string.IndexOf('=');
        s = rs232_receive_string.Substring(pos + 1, rs232_receive_string.Length - pos - 1).TrimEnd();
        try
        {
            value = Convert.ToInt32(s);
            ret = true;
        }
        catch (Exception)
        {
            value = -1;
            ret = false;
        }
        ret_string = s;

        return ret;
    }

    private bool gp22_set_mess_mode(int value, out string ret_string)
    {
        string s;
        int val2;
        bool ret;

        s = "mess_mode=" + Convert.ToString(value) + "\r";
        serialPort1.Write(s);
        s = rs232_get_answer(timeout_command);
        if (s != "OK")
        {
            ret_string = s;
            return false;
        }
        if (gp22_get_mess_mode(out val2, out ret_string))
        {
            if (val2 == value)
                ret = true;
            else
                ret = false;
        }
        else
        {
            ret = false;
        }

        return ret;
    }

    private void gp22_tempMess_setState(bool activated)
    {
            if (activated)
                serialPort1.Write("gp22_pt1000_ON\r");
            else
                serialPort1.Write("gp22_pt1000_OFF\r");

            serialPort1.DiscardInBuffer();
            serialPort1.DiscardOutBuffer();
    }

    private bool gp22_get_anzahl_mw(out int value, out string ret_string)
    {
        string s;
        int pos;
        bool ret = false;

        s = "mw_pro_reg\r";
        serialPort1.Write(s);

        rs232_receive_string = rs232_get_answer(timeout_command);
        if (rs232_receive_string == "Timeout")
        {
            value = -1;
            ret_string = "Timeout";
            return false;
        }
        pos = rs232_receive_string.IndexOf('=');
        s = rs232_receive_string.Substring(pos + 1, rs232_receive_string.Length - pos - 1).TrimEnd();
        try
        {
            value = Convert.ToInt32(s);
            ret = true;
        }
        catch (Exception)
        {
            value = -1;
            ret = false;
        }
        ret_string = s;

        return ret;
    }

    private bool gp22_set_anzahl_mw(int value, out string ret_string)
    {
        string s;
        int val2;
        bool ret;

        s = "mw_pro_reg=" + Convert.ToString(value) + "\r";
        serialPort1.Write(s);
        s = rs232_get_answer(timeout_command);
        if (s != "OK")
        {
            ret_string = s;
            return false;
        }
        if (gp22_get_anzahl_mw(out val2, out ret_string))
        {
            if (val2 == value)
                ret = true;
            else
                ret = false;
        }
        else
        {
            ret = false;
        }

        return ret;
    }

    private bool gp22_get_config_reg_nr(out int value, out string ret_string)
    {
        string s;
        int pos;
        bool ret = false;

        s = "gp22_config_reg_nr\r";
        serialPort1.Write(s);

        rs232_receive_string = rs232_get_answer(timeout_command);
        if (rs232_receive_string == "Timeout")
        {
            value = -1;
            ret_string = "Timeout";
            return false;
        }
        pos = rs232_receive_string.IndexOf('=');
        s = rs232_receive_string.Substring(pos + 1, rs232_receive_string.Length - pos - 1).TrimEnd();
        try
        {
            value = Convert.ToInt32(s);
            ret = true;
        }
        catch (Exception)
        {
            value = -1;
            ret = false;
        }
        ret_string = s;

        return ret;
    }

    private bool gp22_set_config_reg_nr(int value, out string ret_string)
    {
        string s;
        int val2;
        bool ret;

        s = "gp22_config_reg_nr=" + Convert.ToString(value) + "\r";
        serialPort1.Write(s);
        s = rs232_get_answer(timeout_command);
        if (s != "OK")
        {
            ret_string = s;
            return false;
        }
        if (gp22_get_config_reg_nr(out val2, out ret_string))
        {
            if (val2 == value)
                ret = true;
            else
                ret = false;
        }
        else
        {
            ret = false;
        }

        return ret;
    }

    private bool gp22_get_config_reg_data(out string value, out string ret_string)
    {
        string s;
        int pos;
        bool ret = false;

        s = "gp22_config_reg_data\r";
        serialPort1.Write(s);

        rs232_receive_string = rs232_get_answer(timeout_command);
        if (rs232_receive_string == "Timeout")
        {
            value = "";
            ret_string = "Timeout";
            return false;
        }
        pos = rs232_receive_string.IndexOf('=');
        s = rs232_receive_string.Substring(pos + 1, rs232_receive_string.Length - pos - 1).TrimEnd();
        try
        {
            value = s;
            ret = true;
        }
        catch (Exception)
        {
            value = "";
            ret = false;
        }
        ret_string = s;

        return ret;
    }

    private bool gp22_set_config_reg_data(string value, out string ret_string)
    {
        string s;
        string val2;
        bool ret;

        s = "gp22_config_reg_data=" + value + "\r";
        serialPort1.Write(s);
        s = rs232_get_answer(timeout_command);
        if (gp22_get_config_reg_data(out val2, out ret_string))
        {
            if (val2 == value)
                ret = true;
            else
                ret = false;
        }
        else
        {
            ret = false;
        }

        return ret;
    }


    // Sende GP22-Konfiguration an den STM32
    private bool gp22_set_config()
    {
        bool ret = false;
        int i = 0;
        string ret_string = "";

        for (i = 0; i < 7; i++)
        {
            ret = gp22_set_config_reg_nr(i, out ret_string);
            ret = gp22_set_config_reg_data(config_reg[i], out ret_string);
        }

        return ret;
    }

    // Schreibe PCap01-Konfiguration vom STM32 in den PCap01
    private bool gp22_write_config(out string ret_string)
    {
        string s;
        bool ret;

        s = "gp22_write_config\r";
        serialPort1.Write(s);
        s = rs232_get_answer(10 * timeout_command);
        if (s == "OK")
            ret = true;
        else
            ret = false;

        ret_string = s;

        return ret;
    }

    private bool gp22_calibrate_clock(out uint value, out string ret_string)
    {
        string s;
        //int pos;
        bool ret = false;

        value = 0;

        s = "gp22_calibrate_clock\r";
        serialPort1.Write(s);

        rs232_receive_string = rs232_get_answer(timeout_command);
        if (rs232_receive_string == "OK")
            ret = true;
        else
            ret = false;
/*      pos = rs232_receive_string.IndexOf('=');
        s = rs232_receive_string.Substring(pos + 1, rs232_receive_string.Length - pos - 1).TrimEnd();
        try
        {
            value = Convert.ToUInt32(s);
            ret = true;
        }
        catch (Exception)
        {
            value = 0;
            ret = false;
        } */
        ret_string = rs232_receive_string;
        return ret;
    }

    private bool get_temperaturmessung_aktiv(out int value)
    {
        string s;
        int pos;
        bool ret = false;

        s = "temp_mess\r";
        serialPort1.Write(s);

        rs232_receive_string = rs232_get_answer(timeout_command);
        if (rs232_receive_string == "Timeout")
        {
            value = -1;
            return false;
        }
        pos = rs232_receive_string.IndexOf('=');
        s = rs232_receive_string.Substring(pos + 1, rs232_receive_string.Length - pos - 1).TrimEnd();
        try
        {
            value = Convert.ToInt32(s);
            ret = true;
        }
        catch (Exception)
        {
            value = -1;
            ret = false;
        }
        return ret;
    }

    private bool set_temperaturmessung_aktiv(int value)
    {
        string s;
        int val2;
        bool ret;

        s = "temp_mess=" + Convert.ToString(value) + "\r";
        serialPort1.Write(s);
        s = rs232_get_answer(timeout_command);
        if (get_temperaturmessung_aktiv(out val2))
        {
            if (val2 == value)
                ret = true;
            else
                ret = false;
        }
        else
        {
            ret = false;
        }

        return ret;
    }

    private void set_dds(UInt32 value)
    {
        string s;

        s = "dds=" + Convert.ToString(value) + "\r";
        serialPort1.Write(s);
        s = rs232_get_answer(timeout_command);
    }

    private bool gp22_reset()
    {
        string s = "";

        serialPort1.Write("gp22_reset\r");
        s = rs232_get_answer(timeout_command);
        return (s == "OK");
    }

    private bool gp22_initialize()
    {
        string s = "";

        serialPort1.Write("gp22_initialize\r");
        s = rs232_get_answer(timeout_command);
        return (s == "OK");
    }



    private bool lvdt_mess()
    {
        string ret = "";
        int pos = 0;

        if (!rs232_connected)
            return false;

            // 07.11.2017 Daouda
            //serialPort1.Write("gp22_start\r");  // gp22_results
        //serialPort1.DiscardInBuffer();
        rs232_receive_string = rs232_get_answer(timeout_command);

        if (rs232_receive_string == "Timeout")
        {
            ret = "Timeout";
        }
        else
        {
            result_strings = rs232_receive_string.Split(' ');
        }

        return true;
    }

    private void lvdt_start()
    {
        if (!rs232_connected)
            return;

        serialPort1.Write("gp22_start\r");
    }

    private void lvdt_stop()
    {
        if (!rs232_connected)
            return;

        serialPort1.Write("gp22_stop\r");
    }

    private void FrmMain_Shown(object sender, EventArgs e)
    {
      this.Text = "LVDT-Auswertung, Version: " + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString();
      RefreshRS232Ports();
      //chart.ChartAreas["area_data"].AxisY.Minimum = 0;
      //chart.ChartAreas["area_data"].AxisY.Maximum = 5;
      //chart.ChartAreas["area_data"].AxisY2.Minimum = 0;
      //chart.ChartAreas["area_data"].AxisY2.Maximum = 100;
      //chart.Series["s_lvdt"].Points.AddXY(0, 0);
      //chart.Series["s_temperatur"].Points.AddXY(0, 0);
      cbXAusschnitt.Checked = false;
      cbAutoscaleY.Checked = false;
      comboBoxLVDTtype.SelectedIndex = 0; // Zur Auswahl des LVDT-Typs und des zugehörigen Polynoms (VK 2018 - 03 - 22)
      error_log("Programmstart Version " + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString());
      tbDDS.Text = Convert.ToString(dds);
      if (dds_enable == 0x4711)
        tbDDS.Enabled = true;

      PresetPolynoms(); // Polynome an dieser Stelle vorbelegen (VK 2018 - 03 - 22)
    }

    private void btnConnect_Click(object sender, EventArgs e)
    {
      string ret_string="";

      if (rs232_connected)
      {
        serialPort1.DiscardInBuffer();
        serialPort1.DiscardOutBuffer();
        serialPort1.Close();
        rs232_connected = false;
        btnConnect.Text = "verbinden";
        cbRS232.Enabled = true;
        tbStatus.Text = "nicht verbunden";
        tbOffset.Enabled = false;
        tbFaktorWeg.Enabled = false;
        tbAnzahlMittelwerte.Enabled = false;
        tBmaxAbweichung.Enabled = false;
        btnMessungStart.Enabled = false;
        btnMessungStop.Enabled = false;
        cbMessungSpeichern.Enabled = false;
        cbPhasenverschiebung.Enabled = false;
//        cbLVDT.Enabled = false;
        cbTemperatur.Enabled = false;
        panel3.Enabled = false;
      }
      else
      {
        try
        {
          serialPort1.PortName = cbRS232.Text;
          serialPort1.NewLine = "\r";
          serialPort1.RtsEnable = true;
          serialPort1.WriteTimeout = 10000;
          serialPort1.Open();
          btnConnect.Text = "trennen";
          rs232_connected = true;
          cbRS232.Enabled = false;
          panel3.Enabled = true;
          serialPort1.DiscardInBuffer();
          serialPort1.DiscardOutBuffer();
        }
        catch (Exception ex)
        {
          MessageBox.Show(ex.ToString());
           throw;
        }
      }
      btnConnect.Enabled = true;
      if (rs232_connected)
      {
          mcu_version = get_firmware_version();
          if ((mcu_version.Length < 4) || (mcu_version.Substring(0, 4) != "GP22"))
          {
              MessageBox.Show("falsche Hardware angeschlossen !");
              serialPort1.DiscardInBuffer();
              serialPort1.DiscardOutBuffer();
              serialPort1.Close();
              rs232_connected = false;
              btnConnect.Text = "verbinden";
              cbRS232.Enabled = true;
              btnMessungStart.Enabled = false;
              btnMessungStop.Enabled = false;

              return;
          }

        //  if (!gp22_reset())
        //  {
        //      MessageBox.Show("LVDT Reset fehlgeschlagen !");
        //      return;
        //  }

          if (!gp22_initialize())
          {
              MessageBox.Show("LVDT Initialize fehlgeschlagen !");
              return;
          }

          set_dds(dds);

          if (!konfigurationsdatei_laden())
          {
              MessageBox.Show("Konfigurationsdatei fehlerhaft oder nicht vorhanden !");
              return;
          }

          if (!gp22_calibrate_clock(out gp22_clock_calibration_value, out ret_string))
          {
              MessageBox.Show("Fehler: GP22 Clock Calibration !");
              return;
          }

          tbOffset.Enabled = true;
          tbFaktorWeg.Enabled = true;
          tbAnzahlMittelwerte.Enabled = true;
          btnMessungStart.Enabled = true;
          cbXAusschnitt.Checked = true;
          cbMessungSpeichern.Enabled = true;
          cbPhasenverschiebung.Enabled = true;
          tBmaxAbweichung.Enabled = true;
//          cbLVDT.Enabled = true;
          cbTemperatur.Enabled = true;
          tbStatus.Text = "LVDT bereit";
          cbGrafik.Enabled = true;
          first_meas = true;
      }
    }

    private void linkLabel1_Click(object sender, EventArgs e)
    {
      RefreshRS232Ports();
    }

    private void messung()
    {
          CultureInfo cultureUS = new CultureInfo("en-US");
          string dateiname = "";
          string zeile = "";
          bool first_data = true;
          bool ret = false;
          int i = 0;
          int j = 0;
          UInt32 data = 0;
            // 28.05.2018 Daouda

            UInt32[] rawdat = new UInt32[8];
          //  UInt32[] results = new UInt32[5];
            rawdat[0] = 0;
            rawdat[1] = 0;
            rawdat[2] = 0;
            rawdat[3] = 0;
            rawdat[4] = 0;
            rawdat[5] = 0;
            rawdat[6] = 0;
            rawdat[7] = 0;

            UInt32 std_dev = 0;
          UInt32 status = 0;
          double zeit = 0;
          double lvdt = 0;
          double lvdt_weg_mm = 0;
          double lvdt_alt = 0;
          double lvdt_min = 0;
          double lvdt_max = 0;
          double lvdt_std_dev = 0;
          double temp = 0;
          double temp_min = 0;
          double temp_max = 0;
          double x_new = 0;
          UInt32 anzahl_messungen = 0;
          double stopuhr_ms = 0;
          TimeSpan ts;
          string elapsedTime;
          System.Diagnostics.Stopwatch stopwatch = new System.Diagnostics.Stopwatch();
          stopwatch.Reset();
          stopwatch.Start();

          textBoxMaxAbw.Text = String.Format("{0:f6}", abweichung_max);

        while (messung_laeuft)
        {
          zeit = (double)stopwatch.ElapsedMilliseconds / 1000;
          if (lvdt_mess())
          {
                    //rs232_receive_string = rs232_get_answer(timeout_command);
                    //if (rs232_receive_string != "Timeout")
                    //{
                    //    result_strings = rs232_receive_string.Split(' ');
                    //}

                    //            zeit = zeit + (Convert.ToDouble(result_strings[0]) / 1000000);
                    //              zeit = Convert.ToDouble(result_strings[0]);
                    //              label27.Text = result_strings[0];


            if (messdatei_schreiben)
                strFile.Write(String.Format(cultureUS, "{0:f4}", zeit) + " ");
                data = Convert.ToUInt32(result_strings[1]);

                    // 28.05.2018 Daouda
            rawdat[0] = Convert.ToUInt32(result_strings[2]);
            rawdat[1] = Convert.ToUInt32(result_strings[3]);
            rawdat[2] = Convert.ToUInt32(result_strings[4]);
            rawdat[3] = Convert.ToUInt32(result_strings[5]);

            if (cbTemperatur.Checked)
            {
                if ( !( Double.TryParse(result_strings[6], tempStyle, cultureUS, out temp ) ) )
                    temp = -273.15;
                rawdat[4] = Convert.ToUInt32(result_strings[7]);
                rawdat[5] = Convert.ToUInt32(result_strings[8]);
                rawdat[6] = Convert.ToUInt32(result_strings[9]);
                rawdat[7] = Convert.ToUInt32(result_strings[10]);
                std_dev = Convert.ToUInt32(result_strings[11]);
                status = Convert.ToUInt32(result_strings[12]);        
            }
            else
            {
                std_dev = Convert.ToUInt32(result_strings[6]);
                status = Convert.ToUInt32(result_strings[7]);
            }
            lvdt_roh = ((double)((data >> 16) & 0x0000FFFF) + ((double)(data & 0x0000FFFF) / (double)(((UInt32)1) << 16))) / gp22_clock;
            lvdt = lvdt_roh - offset;
            lvdt_std_dev = ((double)(std_dev >> 16) + ((double)(std_dev & 0x0000FFFF) / (double)(((UInt32)1) << 16))) / gp22_clock;
            if (cbPhasenverschiebung.Checked)
            {
                lvdt = 180 * (lvdt - dds / 1000000);
            }

            if (first_meas)
            {
                first_meas = false;

                lvdt_min = lvdt;
                lvdt_max = lvdt;
                temp_min = temp;
                temp_max = temp;

                lvdt_alt = lvdt;
                ausreisser_anzahl = 0;
            }
            else
            {
                /*
                if (offsetGeradeGenullt)
                {
                            offsetGeradeGenullt = false;
                            lvdt_alt = lvdt;
                } */

                // Korrektur der Ausreißer, muss noch im µC-Programm korrigiert werden (langsamere Kommunikation mit GP22)
                if ((lvdt > (lvdt_alt + abweichung_max)) || (lvdt < (lvdt_alt - abweichung_max)))
                {
                    lvdt = lvdt_alt;
                    ausreisser_anzahl++;
                }
                else lvdt_alt = lvdt;

                if (lvdt < lvdt_min)
                    lvdt_min = lvdt;
                if (lvdt > lvdt_max)
                    lvdt_max = lvdt;
                if (temp < temp_min)
                    temp_min = temp;
                if (temp > temp_max)
                    temp_max = temp;
            }
            tbLVDT.Text = String.Format("{0:f6}", lvdt);
            int polyIndex = comboBoxLVDTtype.SelectedIndex;
            lvdt_weg_mm =  // hier Polynom eingetragen um Messbereich zu erweitern (VK 2018-03-22)
                    Polynoms[polyIndex].a * Math.Pow(lvdt, 3) +
                    Polynoms[polyIndex].b * Math.Pow(lvdt, 2) +
                    Polynoms[polyIndex].c * lvdt +
                    Polynoms[polyIndex].d;
            /* lvdt_weg_mm =  // hier Polynom eingetragen um Messbereich zu erweitern (VK 2018-03-22)
                    Polynoms[polyIndex].a *lvdt+
                    Polynoms[polyIndex].b-60;*/
                          
            //-58.3
            tbMesswertMM.Text = String.Format("{0:f6}", lvdt_weg_mm);

            tbLVDTMinimum.Text = String.Format("{0:f6}", lvdt_min);
            tbLVDTMaximum.Text = String.Format("{0:f6}", lvdt_max);
            tBAusreisserZahl.Text = String.Format("{0}", ausreisser_anzahl);

            if (cbTemperatur.Checked)
            {
                tbTemperatur.Text = String.Format("{0:f2}", temp);
                tbTemperaturMinimum.Text = String.Format("{0:f2}", temp_min);
                tbTemperaturMaximum.Text = String.Format("{0:f2}", temp_max);
            }
            tbLVDTStdDev.Text = String.Format("{0:f6}", lvdt_std_dev);
            tbStatusregister.Text = String.Format("0x{0:X4}", status);

            if (cbGrafik.Checked == true)
            {
                rppl_lvdt.Add(zeit / 60, lvdt);
                zgc.AxisChange();
                if (stopwatch.ElapsedMilliseconds%10==0)
                  Refresh();


                //chart.Series["s_lvdt"].Points.AddXY(zeit / 60, lvdt);
                //if (cbTemperatur.Checked)
                //  chart.Series["s_temperatur"].Points.AddXY(zeit / 60, temp);
                //if (cbXAusschnitt.Checked && (chart.Series["s_lvdt"].Points.Count > 1))
                //{
                //    x_new = chart.Series["s_lvdt"].Points[chart.Series["s_lvdt"].Points.Count - 1].XValue;
                //    if (x_new >= chart.ChartAreas["area_data"].AxisX.Maximum)
                //    {
                //        chart.ChartAreas["area_data"].AxisX.Maximum = x_new;
                //        chart.ChartAreas["area_data"].AxisX.Minimum = x_new - zeitabschnitt;

                //        while ((chart.Series["s_lvdt"].Points[0].XValue <= chart.ChartAreas["area_data"].AxisX.Minimum) && (chart.Series["s_lvdt"].Points.Count > 0))
                //            chart.Series["s_lvdt"].Points.RemoveAt(0);
                //        if (cbTemperatur.Checked)
                //        {
                //            while ((chart.Series["s_temperatur"].Points[0].XValue <= chart.ChartAreas["area_data"].AxisX.Minimum) && (chart.Series["s_temperatur"].Points.Count > 0))
                //                chart.Series["s_temperatur"].Points.RemoveAt(0);
                //        }

                //    }
                //}

                //chart.Series["s_lvdt"].Points.AddXY(zeit / 60, lvdt);
                //if (chart.Series["s_lvdt"].Points[chart.Series["s_lvdt"].Points.Count - 1].XValue >= chart.ChartAreas["area_data"].AxisX.Maximum)
                //{
                //    chart.ChartAreas["area_data"].AxisX.Maximum += 0.1;
                //    chart.ChartAreas["area_data"].AxisX.Minimum += 0.1;
                //    while (chart.Series["s_lvdt"].Points[0].XValue <= chart.ChartAreas["area_data"].AxisX.Minimum)
                //        chart.Series["s_lvdt"].Points.RemoveAt(0);
                //}

                //if (cbTemperatur.Checked)
                //{
                //    chart.Series["s_temperatur"].Points.AddXY(zeit / 60, temp);
                //    if (chart.Series["s_temperatur"].Points[chart.Series["s_temperatur"].Points.Count - 1].XValue >= chart.ChartAreas["area_data"].AxisX.Maximum)
                //    {
                //        chart.ChartAreas["area_data"].AxisX.Maximum += 0.1;
                //        chart.ChartAreas["area_data"].AxisX.Minimum += 0.1;
                //        while (chart.Series["s_temperatur"].Points[0].XValue <= chart.ChartAreas["area_data"].AxisX.Minimum)
                //            chart.Series["s_temperatur"].Points.RemoveAt(0);
                //    }
                //}
            }

            if (messdatei_schreiben)
            {
                 // 28.05.2018 Daouda

                strFile.Write(String.Format(cultureUS, "{0:f6}", lvdt) + " ");
                strFile.Write(String.Format(cultureUS, "{0:f6}", lvdt_weg_mm) + " ");
                strFile.Write(String.Format("{0}", ausreisser_anzahl) + " ");
                strFile.Write(String.Format(cultureUS, "{0:f6}", rawdat[0]) + " ");
                strFile.Write(String.Format(cultureUS, "{0:f6}", rawdat[1]) + " ");
                strFile.Write(String.Format(cultureUS, "{0:f6}", rawdat[2]) + " ");
                strFile.Write(String.Format(cultureUS, "{0:f6}", rawdat[3]) + " ");
                if (cbTemperatur.Checked)
                {
                  strFile.Write(String.Format(cultureUS, "{0:f2}", temp) + " ");
                  strFile.Write(String.Format(cultureUS, "{0:f6}", rawdat[4]) + " ");
                  strFile.Write(String.Format(cultureUS, "{0:f6}", rawdat[5]) + " ");
                  strFile.Write(String.Format(cultureUS, "{0:f6}", rawdat[6]) + " ");
                  strFile.Write(String.Format(cultureUS, "{0:f6}", rawdat[7]) + " ");
                }
                strFile.WriteLine(String.Format("0x{0:X4}", status));
            }

            /*****************************/
            if (cbTemperatur.Checked)
            {
             // results[4] = Convert.ToUInt32(result_strings[4]);
             // if (messdatei_schreiben)
             // {
              //  strFile.WriteLine(String.Format("{0:X04}", results[4]));
             // }
             // dgvResults.Rows[i].Cells[1].Value = String.Format("0x{0:X04}", results[4]);
            }

            //results[i] = Convert.ToUInt32(result_strings[i]);
            //if (messdatei_schreiben)
            //{
            //  strFile.WriteLine(String.Format("{0:X04}", results[i]));
            //}
            //dgvResults.Rows[5].Cells[1].Value = String.Format("0x{0:X04}", results[i]);
          if ((messdauer > 0) && (stopwatch.ElapsedMilliseconds >= (messdauer * 60000)))
            messung_laeuft = false;
          else
            if ((anzahl_messwerte > 0) && (anzahl_messungen > anzahl_messwerte))
              messung_laeuft = false;
          ts = stopwatch.Elapsed;
          elapsedTime = String.Format("{0:00}:{1:00}:{2:00}.{3:00}", ts.Hours, ts.Minutes, ts.Seconds, ts.Milliseconds / 10);
          tsLabelMessdauer.Text = "Messzeit: " + elapsedTime;
          anzahl_messungen++;
          tsLabelMesswerte.Text = "Anzahl Messungen: " + anzahl_messungen.ToString();
//          tsLabelMessfrequenz.Text = "Messfrequenz: " + String.Format("{0:0.00}", (anzahl_messungen / zeit)) + " Hz";
          tsLabelMessfrequenz.Text = "Messfrequenz: " + String.Format("{0:0.00}", 1000*((double)anzahl_messungen / (double)stopwatch.ElapsedMilliseconds)) + " Hz";
        }
        Application.DoEvents();
      }




        //    if (first_data)
        //    {
        //        ret = lvdt_get_rohwert(out lvdt);
        //        ret = lvdt_get_flow(out flow);
        //    }

        //    ret = lvdt_get_flow(out flow);
        //    ret = lvdt_get_rohwert(out lvdt);
        //    flow = flow / 1000;
        //    if (cbRohwerte.Checked)
        //      data = lvdt;
        //    else
        //      data = 401 * (lvdt - 522000) / 1000000;
        //    if (ret)
        //    {
        //        l_data.Add(data);
        //        l_data2.Add(flow);
        //        if (first_data)
        //        {
        //            first_data = false;
        //            data_min = data;
        //            data_max = data;
        //            flow_min = flow;
        //            flow_max = flow;
        //        }
        //        if (data < data_min)
        //            data_min = data;
        //        if (data > data_max)
        //            data_max = data;
        //        if (flow < flow_min)
        //            flow_min = flow;
        //        if (flow > flow_max)
        //            flow_max = flow;

        //        zeit = (double)stopwatch.ElapsedMilliseconds / 1000;
        //        l_zeit.Add(zeit);
        //        tbAktuellerWeg.Text = string.Format(culture, "{0:0.000}", data);
        //        tbWegMinimum.Text = string.Format(culture, "{0:0.000}", data_min);
        //        tbWegMaximum.Text = string.Format(culture, "{0:0.000}", data_max);
        //        tbAktuellerFluss.Text = string.Format(culture, "{0:0.000}", flow);
        //        tbFlussMinimum.Text = string.Format(culture, "{0:0.000}", flow_min);
        //        tbFlussMaximum.Text = string.Format(culture, "{0:0.000}", flow_max);
        //        if (cbMessungSpeichern.Checked)
        //        {
        //          zeile = string.Format(culture, "{0:0.000000}", zeit) + " ";
        //          zeile += string.Format(culture, "{0:0.000000}", data);
        //          strFile.WriteLine(zeile);
        //        }
        //        chart.Series["s_daten"].Points.AddXY(zeit / 60, data);
        //        chart.Series["s_flow"].Points.AddXY(zeit / 60, flow);
                
        //        if (cbXAusschnitt.Checked && (chart.Series["s_daten"].Points.Count > 1))
        //        {
        //            x_new = chart.Series["s_daten"].Points[chart.Series["s_daten"].Points.Count - 1].XValue;
        //            if (x_new >= chart.ChartAreas["area_data"].AxisX.Maximum)
        //            {
        //                chart.ChartAreas["area_data"].AxisX.Maximum = x_new;
        //                chart.ChartAreas["area_data"].AxisX.Minimum = x_new - zeitabschnitt;
                        
        //                while ((chart.Series["s_daten"].Points[0].XValue <= chart.ChartAreas["area_data"].AxisX.Minimum) && (chart.Series["s_daten"].Points.Count > 0))
        //                  chart.Series["s_daten"].Points.RemoveAt(0);
        //                while ((chart.Series["s_flow"].Points[0].XValue <= chart.ChartAreas["area_data"].AxisX.Minimum) && (chart.Series["s_flow"].Points.Count > 0))
        //                    chart.Series["s_flow"].Points.RemoveAt(0);
                       
        //            }
        //        }
        //        anzahl_messungen++;
        //        intervallpause = true;
        //        stopuhr_ms = stopwatch.ElapsedMilliseconds;
        //    }
        //    else
        //    {
        //        serialPort1.DiscardOutBuffer();
        //        serialPort1.DiscardInBuffer();
        //    }
        //  if ((stopwatch.ElapsedMilliseconds - stopuhr_ms) >= 1000 * messintervall)
        //    intervallpause = false;

        //  if ((messdauer > 0) && (stopwatch.ElapsedMilliseconds >= (messdauer * 1000)))
        //    messung_laeuft = false;
        //  else
        //    if ((anzahl_messwerte > 0) && (anzahl_messungen > anzahl_messwerte))
        //      messung_laeuft = false;
        //  ts = stopwatch.Elapsed;
        //  elapsedTime = String.Format("{0:00}:{1:00}:{2:00}.{3:00}", ts.Hours, ts.Minutes, ts.Seconds, ts.Milliseconds / 10);
        //  tsLabelMessdauer.Text = "Messdauer: " + elapsedTime + " s";
        //  tsLabelMesswerte.Text = "gemessene Werte: " + Convert.ToString(anzahl_messungen);
        //  Application.DoEvents();
        //}

        //if (cbMessungSpeichern.Checked)
        //    strFile.Close();
        //stopwatch.Stop();
        //stopwatch.Reset();
        //btnMessungStart.Enabled = true;
        //btnMessungStop.Enabled = false;
    }

    private void btnMessungStart_Click(object sender, EventArgs e)
    {
      int i = 0;
      string ret_string = "";

      if (!rs232_connected)
        return;

      // 07.11.2017 Daouda
      gp22_initialize();

      //lvdt_mess();

      gp22_tempMess_setState(cbTemperatur.Checked);

      gp22_set_mess_mode(1, out ret_string);

      try
      {
          offset = Convert.ToDouble(tbOffset.Text, culture);
      }
      catch (Exception)
      {
          MessageBox.Show("Bitte geben Sie eine korrekte Zahl ein !");
          tbOffset.SelectAll();
          tbOffset.Focus();
          return;
      }

      try
      {
          faktor_weg = Convert.ToDouble(tbFaktorWeg.Text, culture);
      }
      catch (Exception)
      {
          MessageBox.Show("Bitte geben Sie eine korrekte Zahl ein !");
          tbFaktorWeg.SelectAll();
          tbFaktorWeg.Focus();
          return;
      }

      try
      {
          anzahl_mittelwert = Convert.ToInt32(tbAnzahlMittelwerte.Text);
      }
      catch (Exception)
      {
          MessageBox.Show("Bitte geben Sie eine korrekte Zahl ein !");
          tbAnzahlMittelwerte.SelectAll();
          tbAnzahlMittelwerte.Focus();
          return;
      }
      gp22_set_anzahl_mw(anzahl_mittelwert, out ret_string);

      try
      {
          y_min = Convert.ToDouble(tbYMin.Text, culture);
      }
      catch (Exception)
      {
          MessageBox.Show("Bitte geben Sie eine gültige Gleitkommazahl ein !");
          tbYMin.SelectAll();
          tbYMin.Focus();
          return;
      }

      try
      {
          y_max = Convert.ToDouble(tbYMax.Text, culture);
      }
      catch (Exception)
      {
          MessageBox.Show("Bitte geben Sie eine gültige Gleitkommazahl ein !");
          tbYMax.SelectAll();
          tbYMax.Focus();
          return;
      }

      if (y_max <= y_min)
      {
          MessageBox.Show("Das Maximum muß größer als das Minimum sein !");
          tbYMax.SelectAll();
          tbYMax.Focus();
          return;
      }

      try
      {
          y2_min = Convert.ToDouble(tbY2Min.Text, culture);
      }
      catch (Exception)
      {
          MessageBox.Show("Bitte geben Sie eine gültige Gleitkommazahl ein !");
          tbY2Min.SelectAll();
          tbY2Min.Focus();
          return;
      }

      try
      {
          y2_max = Convert.ToDouble(tbY2Max.Text, culture);
      }
      catch (Exception)
      {
          MessageBox.Show("Bitte geben Sie eine gültige Gleitkommazahl ein !");
          tbY2Max.SelectAll();
          tbY2Max.Focus();
          return;
      }

      if (y2_max <= y2_min)
      {
          MessageBox.Show("Das Maximum muß größer als das Minimum sein !");
          tbY2Max.SelectAll();
          tbY2Max.Focus();
          return;
      }

      try
      {
          abweichung_max = Math.Abs(Convert.ToDouble(tBmaxAbweichung.Text, culture));
          abweichung_max /= (double)anzahl_mittelwert;
      }
      catch (Exception)
      {
          MessageBox.Show("Bitte geben Sie eine gültige Gleitkommazahl ein !");
          tBmaxAbweichung.SelectAll();
          tBmaxAbweichung.Focus();
          return;
      }

      if (cbXAusschnitt.Checked)
      {
          try
          {
              zeitabschnitt = (Convert.ToDouble(tbZeitausschnitt.Text, culture) / 60);
          }
          catch (Exception)
          {
              MessageBox.Show("Bitte geben Sie eine Zahl > 0 ein !");
              tbZeitausschnitt.SelectAll();
              tbZeitausschnitt.Focus();
              return;
          }

          if (zeitabschnitt <= 0)
          {
              MessageBox.Show("Bitte geben Sie einen Wert >0 ein !");
              tbZeitausschnitt.SelectAll();
              tbZeitausschnitt.Focus();
              return;
          }
      }

      /*
      if (cbTemperatur.Checked)
      {
        set_temperaturmessung_aktiv(1);
      }
      else
        set_temperaturmessung_aktiv(0);
      */

      serialPort1.DiscardInBuffer();
      serialPort1.DiscardOutBuffer();

      if (cbMessungSpeichern.Checked)
      {
          messdatei_auswaehlen();
          if (messdatei == "")
          {
              messdatei_schreiben = false;
          }
          else
          {
              messdatei_schreiben = true;
              strFile = new StreamWriter(messdatei);
              strFile.WriteLine("% Messparameter LVDT-Messung");
              strFile.WriteLine("%");
              strFile.WriteLine("% Version PC-Software: " + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString());
              strFile.WriteLine("% Firmware MCU: " + mcu_version);
              strFile.WriteLine("% GP22-Konfigurationsregister:");
              for (i = 0; i < 7; i++)
              {
                  strFile.WriteLine("% Register " + String.Format("{0:d2}", i) + ": " + config_reg[i]);
              }
              strFile.WriteLine("% DDS-Frequenz [Hz]: " + Convert.ToString(dds));   
              strFile.WriteLine("% TDC-Clock [Hz]: " + Convert.ToString(1000000*gp22_clock));
              strFile.WriteLine("% Offset: " + tbOffset.Text);
              strFile.WriteLine("% Faktor [mm/µs]: " + tbFaktorWeg.Text);
              strFile.WriteLine("% Anzahl 'M' der Messwerte für Mittelwert: " + tbAnzahlMittelwerte.Text);
              strFile.WriteLine("% Grenze 'G' für Abweichung vom vorherigen Wert, ab der korrigiert wird [µs]*: " + tBmaxAbweichung.Text);
              strFile.WriteLine("% * Tatsächlich verwendeter Wert ist: G/M. **AK ist die Anzahl korrigierter Werte.");
              strFile.WriteLine("% Messdatum: " + Convert.ToString(DateTime.Now));
              strFile.WriteLine("%");
              if (cbTemperatur.Checked)
                strFile.WriteLine("% Zeit LVDT Weg AK** Temperatur Status");
              else
                strFile.WriteLine("% Zeit LVDT Weg AK** Status");
          }

      }
      else
      {
          messdatei_schreiben = false;
      }

      //l_zeit.Clear();
      //l_data.Clear();
      //l_data2.Clear();
      if (cbGrafik.Checked)
      {
          gp.CurveList.Remove(li_lvdt);
          rppl_lvdt.Clear();
          li_lvdt = gp.AddCurve("LVDT", rppl_lvdt, Color.Blue, SymbolType.None);
          
          //chart.Series["s_lvdt"].Points.Clear();
          //chart.Series["s_temperatur"].Points.Clear();

          if (cbXAusschnitt.Checked)
          {
              gp.XAxis.Scale.Max = zeitabschnitt;
              gp.XAxis.Scale.Min = 0;
              //chart.ChartAreas["area_data"].AxisX.Maximum = zeitabschnitt;
              //chart.ChartAreas["area_data"].AxisX.Minimum = 0.0;
          }
          else
          {
              gp.XAxis.Scale.MaxAuto = true;
              gp.XAxis.Scale.Min = 0;
              //chart.ChartAreas["area_data"].AxisX.Maximum = double.NaN;
              //chart.ChartAreas["area_data"].AxisX.Minimum = 0.0;
          }
          zgc.AxisChange();
          Refresh();
      }

      dt = DateTime.Now;
      messung_laeuft = true;
      first_meas = true;
      btnConnect.Enabled = false;
      btnMessungStart.Enabled = false;
      btnMessungStop.Enabled = true;
//      tbAnzahlMesswerte.Enabled = false;
//      tbMessdauer.Enabled = false;
      cbMessungSpeichern.Enabled = false;
      programmBeendenToolStripMenuItem.Enabled = false;

      lvdt_start();
      messung();

      if (cbMessungSpeichern.Checked)
        strFile.Close();
      btnConnect.Enabled = true;
      btnMessungStart.Enabled = true;
      btnMessungStop.Enabled = false;
//      tbAnzahlMesswerte.Enabled = true;
//      tbMessdauer.Enabled = true;
      cbMessungSpeichern.Enabled = true;
      programmBeendenToolStripMenuItem.Enabled = true;
    }

    private void btnMessungStop_Click(object sender, EventArgs e)
    {
        lvdt_stop();
        messung_laeuft = false;  
    }

    private void tbMessintervall_KeyPress(object sender, KeyPressEventArgs e)
    {
      NumberFormatInfo numberFormatInfo = System.Globalization.CultureInfo.CurrentCulture.NumberFormat;
      string decimalSeparator = numberFormatInfo.NumberDecimalSeparator;
      string groupSeparator = numberFormatInfo.NumberGroupSeparator;
      string negativeSign = numberFormatInfo.NegativeSign;

      string keyInput = e.KeyChar.ToString();

      if (Char.IsDigit(e.KeyChar))
      {
        // Digits are OK
      }
      else
      {
        if (keyInput.Equals(decimalSeparator))
        {
          // Decimal separator is OK
        }
        else
        {
          if (e.KeyChar == '\b')
          {
            // Backspace key is OK
          }
          else
          {
            // Swallow this invalid key and beep
            e.Handled = true;
            //    MessageBeep();
          }
        }
      }
    }

    private void FrmMain_FormClosing(object sender, FormClosingEventArgs e)
    {
      if (messung_laeuft)
      {
        e.Cancel = true;
        return;
      }
      error_log("Programmende Version " + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString());
    }

    private void programmBeendenToolStripMenuItem_Click(object sender, EventArgs e)
    {
      this.Close();
    }

    private void cbAutoscaleY_CheckedChanged(object sender, EventArgs e)
    {
        if (!cbAutoscaleY.Checked)
        {
            tbYMin.Enabled = true;
            tbYMax.Enabled = true;

//            chart.ChartAreas["area_data"].AxisY.Minimum = y_min;
//            chart.ChartAreas["area_data"].AxisY.Maximum = y_max;

        }
        else
        {
            tbYMin.Enabled = false;
            tbYMax.Enabled = false;
//            chart.ChartAreas["area_data"].AxisY.Maximum = double.NaN;
//            chart.ChartAreas["area_data"].AxisY.Minimum = double.NaN;
        }
    }

    private void cbXAusschnitt_CheckedChanged(object sender, EventArgs e)
    {
        if (cbXAusschnitt.Checked)
        {
            tbZeitausschnitt.Enabled = true;
        }
        else
        {
            tbZeitausschnitt.Enabled = false;
        }
    }

    private void cbAutoscaleY2_CheckedChanged(object sender, EventArgs e)
    {
        //if (!cbAutoscaleY2.Checked)
        //{
        //    tbY2Min.Enabled = true;
        //    tbY2Max.Enabled = true;

        //    chart.ChartAreas["area_data"].AxisY2.Minimum = y2_min;
        //    chart.ChartAreas["area_data"].AxisY2.Maximum = y2_max;

        //}
        //else
        //{
        //    tbY2Min.Enabled = false;
        //    tbY2Max.Enabled = false;
        //    chart.ChartAreas["area_data"].AxisY2.Maximum = double.NaN;
        //    chart.ChartAreas["area_data"].AxisY2.Minimum = double.NaN;
        //}
    }

    private void tbY2Max_KeyDown(object sender, KeyEventArgs e)
    {
        if (e.KeyCode == Keys.Enter)
        {
            e.SuppressKeyPress = true;
            try
            {
                y2_max = Convert.ToDouble(tbY2Max.Text, culture);
            }
            catch (Exception)
            {
                MessageBox.Show("Bitte geben Sie eine gültige Gleitkommazahl ein !");
                tbY2Max.SelectAll();
                tbY2Max.Focus();
                return;
            }

            if (y2_max <= y2_min)
            {
                MessageBox.Show("Das Maximum muß größer als das Minimum sein !");
                tbY2Max.SelectAll();
                tbY2Max.Focus();
                return;
            }
            zgc.AxisChange();
            Refresh();
//            chart.ChartAreas["area_data"].AxisY2.Maximum = y2_max;
        }
    }

    private void tbY2Min_KeyDown(object sender, KeyEventArgs e)
    {
        if (e.KeyCode == Keys.Enter)
        {
            e.SuppressKeyPress = true;
            try
            {
                y2_min = Convert.ToDouble(tbY2Min.Text, culture);
            }
            catch (Exception)
            {
                MessageBox.Show("Bitte geben Sie eine gültige Gleitkommazahl ein !");
                tbY2Min.SelectAll();
                tbY2Min.Focus();
                return;
            }

            if (y2_max <= y2_min)
            {
                MessageBox.Show("Das Maximum muß größer als das Minimum sein !");
                tbY2Min.SelectAll();
                tbY2Min.Focus();
                return;
            }
            zgc.AxisChange();
            Refresh();
//            chart.ChartAreas["area_data"].AxisY2.Minimum = y2_min;
        }
    }

    private void tbYMax_KeyDown(object sender, KeyEventArgs e)
    {
        if (e.KeyCode == Keys.Enter)
        {
            e.SuppressKeyPress = true;
            try
            {
                y_max = Convert.ToDouble(tbYMax.Text, culture);
            }
            catch (Exception)
            {
                MessageBox.Show("Bitte geben Sie eine gültige Gleitkommazahl ein !");
                tbYMax.SelectAll();
                tbYMax.Focus();
                return;
            }

            if (y_max <= y_min)
            {
                MessageBox.Show("Das Maximum muß größer als das Minimum sein !");
                tbYMax.SelectAll();
                tbYMax.Focus();
                return;
            }
            zgc.AxisChange();
            Refresh();
//            chart.ChartAreas["area_data"].AxisY.Maximum = y_max;
        }
    }

    private void tbYMin_KeyDown(object sender, KeyEventArgs e)
    {
        if (e.KeyCode == Keys.Enter)
        {
            e.SuppressKeyPress = true;
            try
            {
                y_min = Convert.ToDouble(tbYMin.Text, culture);
            }
            catch (Exception)
            {
                MessageBox.Show("Bitte geben Sie eine gültige Gleitkommazahl ein !");
                tbYMin.SelectAll();
                tbYMin.Focus();
                return;
            }

            if (y_max <= y_min)
            {
                MessageBox.Show("Das Maximum muß größer als das Minimum sein !");
                tbYMin.SelectAll();
                tbYMin.Focus();
                return;
            }
            gp.YAxis.Scale.Min = y_min;
            zgc.AxisChange();
            Refresh();
//            chart.ChartAreas["area_data"].AxisY.Minimum = y_min;
        }
    }

    private void cbPhasenverschiebung_CheckedChanged(object sender, EventArgs e)
    {
        if (cbPhasenverschiebung.Checked)
        {
            lbEinheitAktuellerMesswert.Text = "°";
            lbEinheitMaximum.Text = "°";
            lbEinheitMinimum.Text = "°";
            lbEinheitOffset.Text = "°";
            lbEinheitAutoscaleYMin.Text = "°";
            lbEinheitAutoscaleYMax.Text = "°";
            lbEinheitStdDev.Text = "°";
            gp.YAxis.Title.Text = "LVDT [°]";
//            chart.ChartAreas["area_data"].AxisY.Title = "LVDT [°]";
        }
        else
        {
            lbEinheitAktuellerMesswert.Text = "µs";
            lbEinheitMaximum.Text = "µs";
            lbEinheitMinimum.Text = "µs";
            lbEinheitOffset.Text = "µs";
            lbEinheitAutoscaleYMin.Text = "µs";
            lbEinheitAutoscaleYMax.Text = "µs";
            lbEinheitStdDev.Text = "µs";
            gp.YAxis.Title.Text = "LVDT [µs]";
//            chart.ChartAreas["area_data"].AxisY.Title = "LVDT [µs]";
        }
        zgc.AxisChange();
        Refresh();
    }

    private void FrmMain_Load(object sender, EventArgs e)
    {
        gp = zgc.GraphPane;
        gp.Title.Text = " ";
        gp.XAxis.Title.Text = "Zeit [min]";
        gp.YAxis.Title.Text = "LVDT [µs]";
//        gp.YAxis.Scale.Min = -1;
    }

        private void buttonNullen_Click(object sender, EventArgs e)
        {
            offset = lvdt_roh;
            tbOffset.Text = offset.ToString(culture);
            //offsetGeradeGenullt = true;
            first_meas = true;
        }

        private void cbTemperatur_CheckedChanged(object sender, EventArgs e)
        {

        }

        /*        private void tbAnzahlMittelwerte_TextChanged(object sender, EventArgs e)
                {
                    string ret_string = "";
                    gp22_set_anzahl_mw(1000, out ret_string);
                } */
    }
}