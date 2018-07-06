namespace LVDT_Auswertung
{
  partial class FrmMain
  {
    /// <summary>
    /// Erforderliche Designervariable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Verwendete Ressourcen bereinigen.
    /// </summary>
    /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Vom Windows Form-Designer generierter Code

    /// <summary>
    /// Erforderliche Methode für die Designerunterstützung.
    /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
    /// </summary>
    private void InitializeComponent()
    {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(FrmMain));
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.dateiToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.programmBeendenToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label7 = new System.Windows.Forms.Label();
            this.tbStatus = new System.Windows.Forms.TextBox();
            this.btnConnect = new System.Windows.Forms.Button();
            this.linkLabel1 = new System.Windows.Forms.LinkLabel();
            this.cbRS232 = new System.Windows.Forms.ComboBox();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.tsLabelMessdauer = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel1 = new System.Windows.Forms.ToolStripStatusLabel();
            this.tsLabelMesswerte = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel2 = new System.Windows.Forms.ToolStripStatusLabel();
            this.tsLabelMessfrequenz = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolStripStatusLabel3 = new System.Windows.Forms.ToolStripStatusLabel();
            this.tsLabelMessdatei = new System.Windows.Forms.ToolStripStatusLabel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.panel3 = new System.Windows.Forms.Panel();
            this.label29 = new System.Windows.Forms.Label();
            this.tbMesswertMM = new System.Windows.Forms.TextBox();
            this.textBoxMaxAbw = new System.Windows.Forms.TextBox();
            this.tBAusreisserZahl = new System.Windows.Forms.TextBox();
            this.label30 = new System.Windows.Forms.Label();
            this.label27 = new System.Windows.Forms.Label();
            this.lbEinheitStdDev = new System.Windows.Forms.Label();
            this.tbLVDTStdDev = new System.Windows.Forms.TextBox();
            this.label25 = new System.Windows.Forms.Label();
            this.tbStatusregister = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.tbTemperaturMaximum = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.tbTemperaturMinimum = new System.Windows.Forms.TextBox();
            this.label18 = new System.Windows.Forms.Label();
            this.tbTemperatur = new System.Windows.Forms.TextBox();
            this.label19 = new System.Windows.Forms.Label();
            this.lbEinheitMaximum = new System.Windows.Forms.Label();
            this.lbEinheitMinimum = new System.Windows.Forms.Label();
            this.lbEinheitAktuellerMesswert = new System.Windows.Forms.Label();
            this.tbLVDTMaximum = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.tbLVDTMinimum = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.tbLVDT = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.btnMessungStart = new System.Windows.Forms.Button();
            this.btnMessungStop = new System.Windows.Forms.Button();
            this.panel2 = new System.Windows.Forms.Panel();
            this.buttonNullen = new System.Windows.Forms.Button();
            this.label31 = new System.Windows.Forms.Label();
            this.comboBoxLVDTtype = new System.Windows.Forms.ComboBox();
            this.tbFaktorWeg = new System.Windows.Forms.TextBox();
            this.label22 = new System.Windows.Forms.Label();
            this.tBmaxAbweichung = new System.Windows.Forms.TextBox();
            this.label28 = new System.Windows.Forms.Label();
            this.tbAnzahlMittelwerte = new System.Windows.Forms.TextBox();
            this.label23 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.lbEinheitOffset = new System.Windows.Forms.Label();
            this.tbDDS = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.tbOffset = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.cbLVDT = new System.Windows.Forms.CheckBox();
            this.cbTemperatur = new System.Windows.Forms.CheckBox();
            this.cbPhasenverschiebung = new System.Windows.Forms.CheckBox();
            this.cbMessungSpeichern = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.panel4 = new System.Windows.Forms.Panel();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.zgc = new ZedGraph.ZedGraphControl();
            this.cbGrafik = new System.Windows.Forms.CheckBox();
            this.label26 = new System.Windows.Forms.Label();
            this.label24 = new System.Windows.Forms.Label();
            this.lbEinheitAutoscaleYMax = new System.Windows.Forms.Label();
            this.lbEinheitAutoscaleYMin = new System.Windows.Forms.Label();
            this.tbY2Max = new System.Windows.Forms.TextBox();
            this.label20 = new System.Windows.Forms.Label();
            this.tbY2Min = new System.Windows.Forms.TextBox();
            this.label21 = new System.Windows.Forms.Label();
            this.cbAutoscaleY2 = new System.Windows.Forms.CheckBox();
            this.label14 = new System.Windows.Forms.Label();
            this.tbZeitausschnitt = new System.Windows.Forms.TextBox();
            this.tbYMax = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.tbYMin = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.cbXAusschnitt = new System.Windows.Forms.CheckBox();
            this.cbAutoscaleY = new System.Windows.Forms.CheckBox();
            this.menuStrip1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.panel3.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // serialPort1
            // 
            this.serialPort1.Handshake = System.IO.Ports.Handshake.RequestToSend;
            this.serialPort1.ReadBufferSize = 1000000;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.dateiToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(984, 24);
            this.menuStrip1.TabIndex = 491;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // dateiToolStripMenuItem
            // 
            this.dateiToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.programmBeendenToolStripMenuItem});
            this.dateiToolStripMenuItem.Name = "dateiToolStripMenuItem";
            this.dateiToolStripMenuItem.Size = new System.Drawing.Size(49, 20);
            this.dateiToolStripMenuItem.Text = "Datei";
            // 
            // programmBeendenToolStripMenuItem
            // 
            this.programmBeendenToolStripMenuItem.Name = "programmBeendenToolStripMenuItem";
            this.programmBeendenToolStripMenuItem.Size = new System.Drawing.Size(186, 22);
            this.programmBeendenToolStripMenuItem.Text = "Programm beenden";
            this.programmBeendenToolStripMenuItem.Click += new System.EventHandler(this.programmBeendenToolStripMenuItem_Click);
            // 
            // saveFileDialog1
            // 
            this.saveFileDialog1.CreatePrompt = true;
            this.saveFileDialog1.DefaultExt = "txt";
            this.saveFileDialog1.FileName = "test.txt";
            this.saveFileDialog1.Filter = "Messdateien | *.txt";
            this.saveFileDialog1.InitialDirectory = ".";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.label7);
            this.panel1.Controls.Add(this.tbStatus);
            this.panel1.Controls.Add(this.btnConnect);
            this.panel1.Controls.Add(this.linkLabel1);
            this.panel1.Controls.Add(this.cbRS232);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 24);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(984, 50);
            this.panel1.TabIndex = 499;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(363, 21);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(43, 13);
            this.label7.TabIndex = 501;
            this.label7.Text = "Status";
            // 
            // tbStatus
            // 
            this.tbStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbStatus.Location = new System.Drawing.Point(412, 18);
            this.tbStatus.Name = "tbStatus";
            this.tbStatus.ReadOnly = true;
            this.tbStatus.Size = new System.Drawing.Size(560, 20);
            this.tbStatus.TabIndex = 500;
            // 
            // btnConnect
            // 
            this.btnConnect.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnConnect.Location = new System.Drawing.Point(239, 17);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(75, 21);
            this.btnConnect.TabIndex = 499;
            this.btnConnect.Text = "verbinden";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // linkLabel1
            // 
            this.linkLabel1.AutoSize = true;
            this.linkLabel1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.linkLabel1.Location = new System.Drawing.Point(5, 21);
            this.linkLabel1.Name = "linkLabel1";
            this.linkLabel1.Size = new System.Drawing.Size(121, 13);
            this.linkLabel1.TabIndex = 498;
            this.linkLabel1.TabStop = true;
            this.linkLabel1.Text = "serielle Schnittstelle";
            this.linkLabel1.Click += new System.EventHandler(this.linkLabel1_Click);
            // 
            // cbRS232
            // 
            this.cbRS232.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbRS232.FormattingEnabled = true;
            this.cbRS232.Location = new System.Drawing.Point(132, 17);
            this.cbRS232.Name = "cbRS232";
            this.cbRS232.Size = new System.Drawing.Size(101, 21);
            this.cbRS232.TabIndex = 497;
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsLabelMessdauer,
            this.toolStripStatusLabel1,
            this.tsLabelMesswerte,
            this.toolStripStatusLabel2,
            this.tsLabelMessfrequenz,
            this.toolStripStatusLabel3,
            this.tsLabelMessdatei});
            this.statusStrip1.Location = new System.Drawing.Point(0, 640);
            this.statusStrip1.Margin = new System.Windows.Forms.Padding(0, 5, 0, 0);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(984, 22);
            this.statusStrip1.TabIndex = 501;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // tsLabelMessdauer
            // 
            this.tsLabelMessdauer.Name = "tsLabelMessdauer";
            this.tsLabelMessdauer.Size = new System.Drawing.Size(72, 17);
            this.tsLabelMessdauer.Text = "Messzeit: 0 s";
            // 
            // toolStripStatusLabel1
            // 
            this.toolStripStatusLabel1.AutoSize = false;
            this.toolStripStatusLabel1.Name = "toolStripStatusLabel1";
            this.toolStripStatusLabel1.Size = new System.Drawing.Size(50, 17);
            // 
            // tsLabelMesswerte
            // 
            this.tsLabelMesswerte.Name = "tsLabelMesswerte";
            this.tsLabelMesswerte.Size = new System.Drawing.Size(112, 17);
            this.tsLabelMesswerte.Text = "gemessene Werte: 0";
            // 
            // toolStripStatusLabel2
            // 
            this.toolStripStatusLabel2.AutoSize = false;
            this.toolStripStatusLabel2.Name = "toolStripStatusLabel2";
            this.toolStripStatusLabel2.Size = new System.Drawing.Size(50, 17);
            // 
            // tsLabelMessfrequenz
            // 
            this.tsLabelMessfrequenz.Name = "tsLabelMessfrequenz";
            this.tsLabelMessfrequenz.Size = new System.Drawing.Size(109, 17);
            this.tsLabelMessfrequenz.Text = "Messfrequenz: 0 Hz";
            // 
            // toolStripStatusLabel3
            // 
            this.toolStripStatusLabel3.AutoSize = false;
            this.toolStripStatusLabel3.Name = "toolStripStatusLabel3";
            this.toolStripStatusLabel3.Size = new System.Drawing.Size(50, 17);
            // 
            // tsLabelMessdatei
            // 
            this.tsLabelMessdatei.Name = "tsLabelMessdatei";
            this.tsLabelMessdatei.Size = new System.Drawing.Size(66, 17);
            this.tsLabelMessdatei.Text = "Messdatei: ";
            // 
            // splitContainer1
            // 
            this.splitContainer1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 74);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.panel3);
            this.splitContainer1.Panel1.Controls.Add(this.panel2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.panel4);
            this.splitContainer1.Size = new System.Drawing.Size(984, 566);
            this.splitContainer1.SplitterDistance = 363;
            this.splitContainer1.TabIndex = 502;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.label29);
            this.panel3.Controls.Add(this.tbMesswertMM);
            this.panel3.Controls.Add(this.textBoxMaxAbw);
            this.panel3.Controls.Add(this.tBAusreisserZahl);
            this.panel3.Controls.Add(this.label30);
            this.panel3.Controls.Add(this.label27);
            this.panel3.Controls.Add(this.lbEinheitStdDev);
            this.panel3.Controls.Add(this.tbLVDTStdDev);
            this.panel3.Controls.Add(this.label25);
            this.panel3.Controls.Add(this.tbStatusregister);
            this.panel3.Controls.Add(this.label6);
            this.panel3.Controls.Add(this.label4);
            this.panel3.Controls.Add(this.label15);
            this.panel3.Controls.Add(this.label16);
            this.panel3.Controls.Add(this.tbTemperaturMaximum);
            this.panel3.Controls.Add(this.label17);
            this.panel3.Controls.Add(this.tbTemperaturMinimum);
            this.panel3.Controls.Add(this.label18);
            this.panel3.Controls.Add(this.tbTemperatur);
            this.panel3.Controls.Add(this.label19);
            this.panel3.Controls.Add(this.lbEinheitMaximum);
            this.panel3.Controls.Add(this.lbEinheitMinimum);
            this.panel3.Controls.Add(this.lbEinheitAktuellerMesswert);
            this.panel3.Controls.Add(this.tbLVDTMaximum);
            this.panel3.Controls.Add(this.label13);
            this.panel3.Controls.Add(this.tbLVDTMinimum);
            this.panel3.Controls.Add(this.label11);
            this.panel3.Controls.Add(this.tbLVDT);
            this.panel3.Controls.Add(this.label9);
            this.panel3.Controls.Add(this.label1);
            this.panel3.Controls.Add(this.btnMessungStart);
            this.panel3.Controls.Add(this.btnMessungStop);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel3.Enabled = false;
            this.panel3.Location = new System.Drawing.Point(0, 224);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(359, 338);
            this.panel3.TabIndex = 502;
            // 
            // label29
            // 
            this.label29.AutoSize = true;
            this.label29.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label29.Location = new System.Drawing.Point(319, 35);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(25, 13);
            this.label29.TabIndex = 549;
            this.label29.Text = "mm";
            // 
            // tbMesswertMM
            // 
            this.tbMesswertMM.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbMesswertMM.Location = new System.Drawing.Point(240, 32);
            this.tbMesswertMM.Name = "tbMesswertMM";
            this.tbMesswertMM.ReadOnly = true;
            this.tbMesswertMM.Size = new System.Drawing.Size(72, 20);
            this.tbMesswertMM.TabIndex = 548;
            this.tbMesswertMM.Text = "0";
            this.tbMesswertMM.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // textBoxMaxAbw
            // 
            this.textBoxMaxAbw.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.textBoxMaxAbw.Location = new System.Drawing.Point(237, 136);
            this.textBoxMaxAbw.Name = "textBoxMaxAbw";
            this.textBoxMaxAbw.ReadOnly = true;
            this.textBoxMaxAbw.Size = new System.Drawing.Size(72, 20);
            this.textBoxMaxAbw.TabIndex = 547;
            this.textBoxMaxAbw.Text = "0";
            this.textBoxMaxAbw.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // tBAusreisserZahl
            // 
            this.tBAusreisserZahl.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tBAusreisserZahl.Location = new System.Drawing.Point(130, 136);
            this.tBAusreisserZahl.Name = "tBAusreisserZahl";
            this.tBAusreisserZahl.ReadOnly = true;
            this.tBAusreisserZahl.Size = new System.Drawing.Size(72, 20);
            this.tBAusreisserZahl.TabIndex = 546;
            this.tBAusreisserZahl.Text = "0";
            this.tBAusreisserZahl.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label30
            // 
            this.label30.AutoSize = true;
            this.label30.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label30.Location = new System.Drawing.Point(15, 139);
            this.label30.Name = "label30";
            this.label30.Size = new System.Drawing.Size(106, 13);
            this.label30.TabIndex = 545;
            this.label30.Text = "Anzahl Ausreißer:";
            // 
            // label27
            // 
            this.label27.AutoSize = true;
            this.label27.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label27.Location = new System.Drawing.Point(211, 137);
            this.label27.Name = "label27";
            this.label27.Size = new System.Drawing.Size(17, 17);
            this.label27.TabIndex = 544;
            this.label27.Text = ">";
            // 
            // lbEinheitStdDev
            // 
            this.lbEinheitStdDev.AutoSize = true;
            this.lbEinheitStdDev.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbEinheitStdDev.Location = new System.Drawing.Point(209, 113);
            this.lbEinheitStdDev.Name = "lbEinheitStdDev";
            this.lbEinheitStdDev.Size = new System.Drawing.Size(20, 13);
            this.lbEinheitStdDev.TabIndex = 543;
            this.lbEinheitStdDev.Text = "µs";
            // 
            // tbLVDTStdDev
            // 
            this.tbLVDTStdDev.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbLVDTStdDev.Location = new System.Drawing.Point(130, 110);
            this.tbLVDTStdDev.Name = "tbLVDTStdDev";
            this.tbLVDTStdDev.ReadOnly = true;
            this.tbLVDTStdDev.Size = new System.Drawing.Size(72, 20);
            this.tbLVDTStdDev.TabIndex = 542;
            this.tbLVDTStdDev.Text = "0";
            this.tbLVDTStdDev.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label25.Location = new System.Drawing.Point(26, 113);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(97, 13);
            this.label25.TabIndex = 541;
            this.label25.Text = "LVDT Std.abw.:";
            // 
            // tbStatusregister
            // 
            this.tbStatusregister.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbStatusregister.Location = new System.Drawing.Point(130, 254);
            this.tbStatusregister.Name = "tbStatusregister";
            this.tbStatusregister.ReadOnly = true;
            this.tbStatusregister.Size = new System.Drawing.Size(72, 20);
            this.tbStatusregister.TabIndex = 540;
            this.tbStatusregister.Text = "0";
            this.tbStatusregister.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(31, 257);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(89, 13);
            this.label6.TabIndex = 539;
            this.label6.Text = "Statusregister:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(209, 217);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(20, 13);
            this.label4.TabIndex = 538;
            this.label4.Text = "°C";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label15.Location = new System.Drawing.Point(209, 191);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(20, 13);
            this.label15.TabIndex = 537;
            this.label15.Text = "°C";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label16.Location = new System.Drawing.Point(209, 165);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(20, 13);
            this.label16.TabIndex = 536;
            this.label16.Text = "°C";
            // 
            // tbTemperaturMaximum
            // 
            this.tbTemperaturMaximum.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbTemperaturMaximum.Location = new System.Drawing.Point(130, 214);
            this.tbTemperaturMaximum.Name = "tbTemperaturMaximum";
            this.tbTemperaturMaximum.ReadOnly = true;
            this.tbTemperaturMaximum.Size = new System.Drawing.Size(72, 20);
            this.tbTemperaturMaximum.TabIndex = 535;
            this.tbTemperaturMaximum.Text = "0";
            this.tbTemperaturMaximum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label17.Location = new System.Drawing.Point(58, 217);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(62, 13);
            this.label17.TabIndex = 534;
            this.label17.Text = "Maximum:";
            // 
            // tbTemperaturMinimum
            // 
            this.tbTemperaturMinimum.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbTemperaturMinimum.Location = new System.Drawing.Point(130, 188);
            this.tbTemperaturMinimum.Name = "tbTemperaturMinimum";
            this.tbTemperaturMinimum.ReadOnly = true;
            this.tbTemperaturMinimum.Size = new System.Drawing.Size(72, 20);
            this.tbTemperaturMinimum.TabIndex = 533;
            this.tbTemperaturMinimum.Text = "0";
            this.tbTemperaturMinimum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label18.Location = new System.Drawing.Point(61, 191);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(59, 13);
            this.label18.TabIndex = 532;
            this.label18.Text = "Minimum:";
            // 
            // tbTemperatur
            // 
            this.tbTemperatur.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbTemperatur.Location = new System.Drawing.Point(130, 162);
            this.tbTemperatur.Name = "tbTemperatur";
            this.tbTemperatur.ReadOnly = true;
            this.tbTemperatur.Size = new System.Drawing.Size(72, 20);
            this.tbTemperatur.TabIndex = 531;
            this.tbTemperatur.Text = "0";
            this.tbTemperatur.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label19.Location = new System.Drawing.Point(45, 165);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(75, 13);
            this.label19.TabIndex = 530;
            this.label19.Text = "Temperatur:";
            // 
            // lbEinheitMaximum
            // 
            this.lbEinheitMaximum.AutoSize = true;
            this.lbEinheitMaximum.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbEinheitMaximum.Location = new System.Drawing.Point(209, 87);
            this.lbEinheitMaximum.Name = "lbEinheitMaximum";
            this.lbEinheitMaximum.Size = new System.Drawing.Size(20, 13);
            this.lbEinheitMaximum.TabIndex = 529;
            this.lbEinheitMaximum.Text = "µs";
            // 
            // lbEinheitMinimum
            // 
            this.lbEinheitMinimum.AutoSize = true;
            this.lbEinheitMinimum.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbEinheitMinimum.Location = new System.Drawing.Point(209, 61);
            this.lbEinheitMinimum.Name = "lbEinheitMinimum";
            this.lbEinheitMinimum.Size = new System.Drawing.Size(20, 13);
            this.lbEinheitMinimum.TabIndex = 528;
            this.lbEinheitMinimum.Text = "µs";
            // 
            // lbEinheitAktuellerMesswert
            // 
            this.lbEinheitAktuellerMesswert.AutoSize = true;
            this.lbEinheitAktuellerMesswert.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbEinheitAktuellerMesswert.Location = new System.Drawing.Point(209, 35);
            this.lbEinheitAktuellerMesswert.Name = "lbEinheitAktuellerMesswert";
            this.lbEinheitAktuellerMesswert.Size = new System.Drawing.Size(20, 13);
            this.lbEinheitAktuellerMesswert.TabIndex = 527;
            this.lbEinheitAktuellerMesswert.Text = "µs";
            // 
            // tbLVDTMaximum
            // 
            this.tbLVDTMaximum.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbLVDTMaximum.Location = new System.Drawing.Point(130, 84);
            this.tbLVDTMaximum.Name = "tbLVDTMaximum";
            this.tbLVDTMaximum.ReadOnly = true;
            this.tbLVDTMaximum.Size = new System.Drawing.Size(72, 20);
            this.tbLVDTMaximum.TabIndex = 525;
            this.tbLVDTMaximum.Text = "0";
            this.tbLVDTMaximum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label13.Location = new System.Drawing.Point(26, 87);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(98, 13);
            this.label13.TabIndex = 524;
            this.label13.Text = "LVDT Maximum:";
            // 
            // tbLVDTMinimum
            // 
            this.tbLVDTMinimum.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbLVDTMinimum.Location = new System.Drawing.Point(130, 58);
            this.tbLVDTMinimum.Name = "tbLVDTMinimum";
            this.tbLVDTMinimum.ReadOnly = true;
            this.tbLVDTMinimum.Size = new System.Drawing.Size(72, 20);
            this.tbLVDTMinimum.TabIndex = 522;
            this.tbLVDTMinimum.Text = "0";
            this.tbLVDTMinimum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label11.Location = new System.Drawing.Point(30, 61);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(95, 13);
            this.label11.TabIndex = 521;
            this.label11.Text = "LVDT Minimum:";
            // 
            // tbLVDT
            // 
            this.tbLVDT.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbLVDT.Location = new System.Drawing.Point(130, 32);
            this.tbLVDT.Name = "tbLVDT";
            this.tbLVDT.ReadOnly = true;
            this.tbLVDT.Size = new System.Drawing.Size(72, 20);
            this.tbLVDT.TabIndex = 519;
            this.tbLVDT.Text = "0";
            this.tbLVDT.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label9.Location = new System.Drawing.Point(24, 35);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(100, 13);
            this.label9.TabIndex = 518;
            this.label9.Text = "LVDT Messwert:";
            // 
            // label1
            // 
            this.label1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(192)))));
            this.label1.Dock = System.Windows.Forms.DockStyle.Top;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.ForeColor = System.Drawing.Color.White;
            this.label1.Location = new System.Drawing.Point(0, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(359, 19);
            this.label1.TabIndex = 504;
            this.label1.Text = "Messung";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // btnMessungStart
            // 
            this.btnMessungStart.Enabled = false;
            this.btnMessungStart.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnMessungStart.Location = new System.Drawing.Point(57, 307);
            this.btnMessungStart.Name = "btnMessungStart";
            this.btnMessungStart.Size = new System.Drawing.Size(91, 23);
            this.btnMessungStart.TabIndex = 501;
            this.btnMessungStart.Text = "Start";
            this.btnMessungStart.UseVisualStyleBackColor = true;
            this.btnMessungStart.Click += new System.EventHandler(this.btnMessungStart_Click);
            // 
            // btnMessungStop
            // 
            this.btnMessungStop.Enabled = false;
            this.btnMessungStop.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.btnMessungStop.Location = new System.Drawing.Point(179, 307);
            this.btnMessungStop.Name = "btnMessungStop";
            this.btnMessungStop.Size = new System.Drawing.Size(91, 23);
            this.btnMessungStop.TabIndex = 502;
            this.btnMessungStop.Text = "Stop";
            this.btnMessungStop.UseVisualStyleBackColor = true;
            this.btnMessungStop.Click += new System.EventHandler(this.btnMessungStop_Click);
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.buttonNullen);
            this.panel2.Controls.Add(this.label31);
            this.panel2.Controls.Add(this.comboBoxLVDTtype);
            this.panel2.Controls.Add(this.tbFaktorWeg);
            this.panel2.Controls.Add(this.label22);
            this.panel2.Controls.Add(this.tBmaxAbweichung);
            this.panel2.Controls.Add(this.label28);
            this.panel2.Controls.Add(this.tbAnzahlMittelwerte);
            this.panel2.Controls.Add(this.label23);
            this.panel2.Controls.Add(this.label2);
            this.panel2.Controls.Add(this.lbEinheitOffset);
            this.panel2.Controls.Add(this.tbDDS);
            this.panel2.Controls.Add(this.label8);
            this.panel2.Controls.Add(this.tbOffset);
            this.panel2.Controls.Add(this.label5);
            this.panel2.Controls.Add(this.cbLVDT);
            this.panel2.Controls.Add(this.cbTemperatur);
            this.panel2.Controls.Add(this.cbPhasenverschiebung);
            this.panel2.Controls.Add(this.cbMessungSpeichern);
            this.panel2.Controls.Add(this.label3);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(359, 224);
            this.panel2.TabIndex = 501;
            // 
            // buttonNullen
            // 
            this.buttonNullen.Location = new System.Drawing.Point(281, 109);
            this.buttonNullen.Name = "buttonNullen";
            this.buttonNullen.Size = new System.Drawing.Size(75, 23);
            this.buttonNullen.TabIndex = 552;
            this.buttonNullen.Text = "Nullen";
            this.buttonNullen.UseVisualStyleBackColor = true;
            this.buttonNullen.Click += new System.EventHandler(this.buttonNullen_Click);
            // 
            // label31
            // 
            this.label31.AutoSize = true;
            this.label31.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label31.Location = new System.Drawing.Point(219, 31);
            this.label31.Name = "label31";
            this.label31.Size = new System.Drawing.Size(68, 13);
            this.label31.TabIndex = 551;
            this.label31.Text = "LVDT-Typ:";
            // 
            // comboBoxLVDTtype
            // 
            this.comboBoxLVDTtype.FormattingEnabled = true;
            this.comboBoxLVDTtype.Items.AddRange(new object[] {
            "200mm",
            "150mm",
            "100mm",
            "50mm",
            "50Si",
            "50So"});
            this.comboBoxLVDTtype.Location = new System.Drawing.Point(289, 28);
            this.comboBoxLVDTtype.MaxDropDownItems = 6;
            this.comboBoxLVDTtype.Name = "comboBoxLVDTtype";
            this.comboBoxLVDTtype.Size = new System.Drawing.Size(67, 21);
            this.comboBoxLVDTtype.TabIndex = 550;
            this.comboBoxLVDTtype.Text = "200mm";
            // 
            // tbFaktorWeg
            // 
            this.tbFaktorWeg.Enabled = false;
            this.tbFaktorWeg.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbFaktorWeg.Location = new System.Drawing.Point(130, 137);
            this.tbFaktorWeg.Name = "tbFaktorWeg";
            this.tbFaktorWeg.Size = new System.Drawing.Size(72, 20);
            this.tbFaktorWeg.TabIndex = 549;
            this.tbFaktorWeg.Text = "2740";
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label22.Location = new System.Drawing.Point(208, 192);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(20, 13);
            this.label22.TabIndex = 548;
            this.label22.Text = "µs";
            // 
            // tBmaxAbweichung
            // 
            this.tBmaxAbweichung.Enabled = false;
            this.tBmaxAbweichung.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tBmaxAbweichung.Location = new System.Drawing.Point(130, 189);
            this.tBmaxAbweichung.Name = "tBmaxAbweichung";
            this.tBmaxAbweichung.Size = new System.Drawing.Size(72, 20);
            this.tBmaxAbweichung.TabIndex = 547;
            this.tBmaxAbweichung.Text = "1,1";
            // 
            // label28
            // 
            this.label28.AutoSize = true;
            this.label28.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label28.Location = new System.Drawing.Point(9, 192);
            this.label28.Name = "label28";
            this.label28.Size = new System.Drawing.Size(111, 13);
            this.label28.TabIndex = 546;
            this.label28.Text = "Max. Abweichung:";
            // 
            // tbAnzahlMittelwerte
            // 
            this.tbAnzahlMittelwerte.Enabled = false;
            this.tbAnzahlMittelwerte.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbAnzahlMittelwerte.Location = new System.Drawing.Point(130, 163);
            this.tbAnzahlMittelwerte.Name = "tbAnzahlMittelwerte";
            this.tbAnzahlMittelwerte.Size = new System.Drawing.Size(72, 20);
            this.tbAnzahlMittelwerte.TabIndex = 545;
            this.tbAnzahlMittelwerte.Text = "10";
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label23.Location = new System.Drawing.Point(15, 166);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(105, 13);
            this.label23.TabIndex = 544;
            this.label23.Text = "Anzahl Mittelung:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(208, 140);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(44, 13);
            this.label2.TabIndex = 543;
            this.label2.Text = "mm/µs";
            // 
            // lbEinheitOffset
            // 
            this.lbEinheitOffset.AutoSize = true;
            this.lbEinheitOffset.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbEinheitOffset.Location = new System.Drawing.Point(208, 114);
            this.lbEinheitOffset.Name = "lbEinheitOffset";
            this.lbEinheitOffset.Size = new System.Drawing.Size(20, 13);
            this.lbEinheitOffset.TabIndex = 510;
            this.lbEinheitOffset.Text = "µs";
            // 
            // tbDDS
            // 
            this.tbDDS.Enabled = false;
            this.tbDDS.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbDDS.Location = new System.Drawing.Point(289, 53);
            this.tbDDS.Name = "tbDDS";
            this.tbDDS.Size = new System.Drawing.Size(67, 20);
            this.tbDDS.TabIndex = 542;
            this.tbDDS.Text = "0";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label8.Location = new System.Drawing.Point(24, 140);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(96, 13);
            this.label8.TabIndex = 541;
            this.label8.Text = "Faktor für Weg:";
            // 
            // tbOffset
            // 
            this.tbOffset.Enabled = false;
            this.tbOffset.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbOffset.Location = new System.Drawing.Point(130, 111);
            this.tbOffset.Name = "tbOffset";
            this.tbOffset.Size = new System.Drawing.Size(72, 20);
            this.tbOffset.TabIndex = 509;
            this.tbOffset.Text = "-1,149";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(76, 114);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(45, 13);
            this.label5.TabIndex = 508;
            this.label5.Text = "Offset:";
            // 
            // cbLVDT
            // 
            this.cbLVDT.AutoSize = true;
            this.cbLVDT.Checked = true;
            this.cbLVDT.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbLVDT.Enabled = false;
            this.cbLVDT.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbLVDT.Location = new System.Drawing.Point(6, 88);
            this.cbLVDT.Name = "cbLVDT";
            this.cbLVDT.Size = new System.Drawing.Size(97, 17);
            this.cbLVDT.TabIndex = 507;
            this.cbLVDT.Text = "LVDT-Signal";
            this.cbLVDT.UseVisualStyleBackColor = true;
            // 
            // cbTemperatur
            // 
            this.cbTemperatur.AutoSize = true;
            this.cbTemperatur.Enabled = false;
            this.cbTemperatur.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbTemperatur.Location = new System.Drawing.Point(141, 88);
            this.cbTemperatur.Name = "cbTemperatur";
            this.cbTemperatur.Size = new System.Drawing.Size(90, 17);
            this.cbTemperatur.TabIndex = 506;
            this.cbTemperatur.Text = "Temperatur";
            this.cbTemperatur.UseVisualStyleBackColor = true;
            this.cbTemperatur.CheckedChanged += new System.EventHandler(this.cbTemperatur_CheckedChanged);
            // 
            // cbPhasenverschiebung
            // 
            this.cbPhasenverschiebung.AutoSize = true;
            this.cbPhasenverschiebung.Enabled = false;
            this.cbPhasenverschiebung.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbPhasenverschiebung.Location = new System.Drawing.Point(6, 55);
            this.cbPhasenverschiebung.Name = "cbPhasenverschiebung";
            this.cbPhasenverschiebung.Size = new System.Drawing.Size(284, 17);
            this.cbPhasenverschiebung.TabIndex = 505;
            this.cbPhasenverschiebung.Text = "Messdaten als Phasenverschiebung anzeigen";
            this.cbPhasenverschiebung.UseVisualStyleBackColor = true;
            this.cbPhasenverschiebung.CheckedChanged += new System.EventHandler(this.cbPhasenverschiebung_CheckedChanged);
            // 
            // cbMessungSpeichern
            // 
            this.cbMessungSpeichern.AutoSize = true;
            this.cbMessungSpeichern.Enabled = false;
            this.cbMessungSpeichern.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbMessungSpeichern.Location = new System.Drawing.Point(6, 32);
            this.cbMessungSpeichern.Name = "cbMessungSpeichern";
            this.cbMessungSpeichern.Size = new System.Drawing.Size(146, 17);
            this.cbMessungSpeichern.TabIndex = 504;
            this.cbMessungSpeichern.Text = "Messdaten speichern";
            this.cbMessungSpeichern.UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this.label3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(192)))));
            this.label3.Dock = System.Windows.Forms.DockStyle.Top;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point(0, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(359, 19);
            this.label3.TabIndex = 503;
            this.label3.Text = "Einstellungen";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel4
            // 
            this.panel4.Controls.Add(this.splitContainer2);
            this.panel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel4.Location = new System.Drawing.Point(0, 0);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(613, 562);
            this.panel4.TabIndex = 26;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.zgc);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.cbGrafik);
            this.splitContainer2.Panel2.Controls.Add(this.label26);
            this.splitContainer2.Panel2.Controls.Add(this.label24);
            this.splitContainer2.Panel2.Controls.Add(this.lbEinheitAutoscaleYMax);
            this.splitContainer2.Panel2.Controls.Add(this.lbEinheitAutoscaleYMin);
            this.splitContainer2.Panel2.Controls.Add(this.tbY2Max);
            this.splitContainer2.Panel2.Controls.Add(this.label20);
            this.splitContainer2.Panel2.Controls.Add(this.tbY2Min);
            this.splitContainer2.Panel2.Controls.Add(this.label21);
            this.splitContainer2.Panel2.Controls.Add(this.cbAutoscaleY2);
            this.splitContainer2.Panel2.Controls.Add(this.label14);
            this.splitContainer2.Panel2.Controls.Add(this.tbZeitausschnitt);
            this.splitContainer2.Panel2.Controls.Add(this.tbYMax);
            this.splitContainer2.Panel2.Controls.Add(this.label10);
            this.splitContainer2.Panel2.Controls.Add(this.tbYMin);
            this.splitContainer2.Panel2.Controls.Add(this.label12);
            this.splitContainer2.Panel2.Controls.Add(this.cbXAusschnitt);
            this.splitContainer2.Panel2.Controls.Add(this.cbAutoscaleY);
            this.splitContainer2.Size = new System.Drawing.Size(613, 562);
            this.splitContainer2.SplitterDistance = 441;
            this.splitContainer2.TabIndex = 0;
            // 
            // zgc
            // 
            this.zgc.Dock = System.Windows.Forms.DockStyle.Fill;
            this.zgc.Location = new System.Drawing.Point(0, 0);
            this.zgc.Name = "zgc";
            this.zgc.ScrollGrace = 0D;
            this.zgc.ScrollMaxX = 0D;
            this.zgc.ScrollMaxY = 0D;
            this.zgc.ScrollMaxY2 = 0D;
            this.zgc.ScrollMinX = 0D;
            this.zgc.ScrollMinY = 0D;
            this.zgc.ScrollMinY2 = 0D;
            this.zgc.Size = new System.Drawing.Size(613, 441);
            this.zgc.TabIndex = 0;
            // 
            // cbGrafik
            // 
            this.cbGrafik.AutoSize = true;
            this.cbGrafik.Enabled = false;
            this.cbGrafik.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbGrafik.Location = new System.Drawing.Point(12, 10);
            this.cbGrafik.Name = "cbGrafik";
            this.cbGrafik.Size = new System.Drawing.Size(115, 17);
            this.cbGrafik.TabIndex = 563;
            this.cbGrafik.Text = "Grafik anzeigen";
            this.cbGrafik.UseVisualStyleBackColor = true;
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label26.Location = new System.Drawing.Point(587, 88);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(20, 13);
            this.label26.TabIndex = 562;
            this.label26.Text = "°C";
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label24.Location = new System.Drawing.Point(426, 90);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(20, 13);
            this.label24.TabIndex = 561;
            this.label24.Text = "°C";
            // 
            // lbEinheitAutoscaleYMax
            // 
            this.lbEinheitAutoscaleYMax.AutoSize = true;
            this.lbEinheitAutoscaleYMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbEinheitAutoscaleYMax.Location = new System.Drawing.Point(587, 37);
            this.lbEinheitAutoscaleYMax.Name = "lbEinheitAutoscaleYMax";
            this.lbEinheitAutoscaleYMax.Size = new System.Drawing.Size(20, 13);
            this.lbEinheitAutoscaleYMax.TabIndex = 560;
            this.lbEinheitAutoscaleYMax.Text = "µs";
            // 
            // lbEinheitAutoscaleYMin
            // 
            this.lbEinheitAutoscaleYMin.AutoSize = true;
            this.lbEinheitAutoscaleYMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lbEinheitAutoscaleYMin.Location = new System.Drawing.Point(426, 38);
            this.lbEinheitAutoscaleYMin.Name = "lbEinheitAutoscaleYMin";
            this.lbEinheitAutoscaleYMin.Size = new System.Drawing.Size(20, 13);
            this.lbEinheitAutoscaleYMin.TabIndex = 559;
            this.lbEinheitAutoscaleYMin.Text = "µs";
            // 
            // tbY2Max
            // 
            this.tbY2Max.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbY2Max.Location = new System.Drawing.Point(511, 85);
            this.tbY2Max.Name = "tbY2Max";
            this.tbY2Max.Size = new System.Drawing.Size(70, 20);
            this.tbY2Max.TabIndex = 558;
            this.tbY2Max.Text = "100";
            this.tbY2Max.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.tbY2Max.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbY2Max_KeyDown);
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label20.Location = new System.Drawing.Point(476, 89);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(34, 13);
            this.label20.TabIndex = 557;
            this.label20.Text = "Max.";
            // 
            // tbY2Min
            // 
            this.tbY2Min.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbY2Min.Location = new System.Drawing.Point(350, 86);
            this.tbY2Min.Name = "tbY2Min";
            this.tbY2Min.Size = new System.Drawing.Size(70, 20);
            this.tbY2Min.TabIndex = 556;
            this.tbY2Min.Text = "0";
            this.tbY2Min.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.tbY2Min.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbY2Min_KeyDown);
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label21.Location = new System.Drawing.Point(315, 90);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(31, 13);
            this.label21.TabIndex = 555;
            this.label21.Text = "Min.";
            // 
            // cbAutoscaleY2
            // 
            this.cbAutoscaleY2.AutoSize = true;
            this.cbAutoscaleY2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbAutoscaleY2.Location = new System.Drawing.Point(313, 61);
            this.cbAutoscaleY2.Name = "cbAutoscaleY2";
            this.cbAutoscaleY2.Size = new System.Drawing.Size(178, 17);
            this.cbAutoscaleY2.TabIndex = 554;
            this.cbAutoscaleY2.Text = "Autoskalierung Temperatur";
            this.cbAutoscaleY2.UseVisualStyleBackColor = true;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label14.Location = new System.Drawing.Point(108, 88);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(13, 13);
            this.label14.TabIndex = 553;
            this.label14.Text = "s";
            // 
            // tbZeitausschnitt
            // 
            this.tbZeitausschnitt.Enabled = false;
            this.tbZeitausschnitt.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbZeitausschnitt.Location = new System.Drawing.Point(32, 85);
            this.tbZeitausschnitt.Name = "tbZeitausschnitt";
            this.tbZeitausschnitt.Size = new System.Drawing.Size(70, 20);
            this.tbZeitausschnitt.TabIndex = 552;
            this.tbZeitausschnitt.Text = "120";
            this.tbZeitausschnitt.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // tbYMax
            // 
            this.tbYMax.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbYMax.Location = new System.Drawing.Point(511, 34);
            this.tbYMax.Name = "tbYMax";
            this.tbYMax.Size = new System.Drawing.Size(70, 20);
            this.tbYMax.TabIndex = 551;
            this.tbYMax.Text = "5";
            this.tbYMax.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.tbYMax.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbYMax_KeyDown);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label10.Location = new System.Drawing.Point(476, 38);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(34, 13);
            this.label10.TabIndex = 550;
            this.label10.Text = "Max.";
            // 
            // tbYMin
            // 
            this.tbYMin.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.tbYMin.Location = new System.Drawing.Point(350, 35);
            this.tbYMin.Name = "tbYMin";
            this.tbYMin.Size = new System.Drawing.Size(70, 20);
            this.tbYMin.TabIndex = 549;
            this.tbYMin.Text = "0";
            this.tbYMin.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.tbYMin.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbYMin_KeyDown);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label12.Location = new System.Drawing.Point(315, 39);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(31, 13);
            this.label12.TabIndex = 548;
            this.label12.Text = "Min.";
            // 
            // cbXAusschnitt
            // 
            this.cbXAusschnitt.AutoSize = true;
            this.cbXAusschnitt.Checked = true;
            this.cbXAusschnitt.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbXAusschnitt.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbXAusschnitt.Location = new System.Drawing.Point(12, 60);
            this.cbXAusschnitt.Name = "cbXAusschnitt";
            this.cbXAusschnitt.Size = new System.Drawing.Size(136, 17);
            this.cbXAusschnitt.TabIndex = 547;
            this.cbXAusschnitt.Text = "Ausschnitt X-Achse";
            this.cbXAusschnitt.UseVisualStyleBackColor = true;
            this.cbXAusschnitt.CheckedChanged += new System.EventHandler(this.cbXAusschnitt_CheckedChanged);
            // 
            // cbAutoscaleY
            // 
            this.cbAutoscaleY.AutoSize = true;
            this.cbAutoscaleY.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.cbAutoscaleY.Location = new System.Drawing.Point(313, 10);
            this.cbAutoscaleY.Name = "cbAutoscaleY";
            this.cbAutoscaleY.Size = new System.Drawing.Size(146, 17);
            this.cbAutoscaleY.TabIndex = 546;
            this.cbAutoscaleY.Text = "Autoskalierung LVDT";
            this.cbAutoscaleY.UseVisualStyleBackColor = true;
            this.cbAutoscaleY.CheckedChanged += new System.EventHandler(this.cbAutoscaleY_CheckedChanged);
            // 
            // FrmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.ClientSize = new System.Drawing.Size(984, 662);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.menuStrip1);
            this.DoubleBuffered = true;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "FrmMain";
            this.Text = "LVDT-Auswertung";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FrmMain_FormClosing);
            this.Load += new System.EventHandler(this.FrmMain_Load);
            this.Shown += new System.EventHandler(this.FrmMain_Shown);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel4.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            this.splitContainer2.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

    }

    #endregion

    private System.IO.Ports.SerialPort serialPort1;
    private System.Windows.Forms.MenuStrip menuStrip1;
    private System.Windows.Forms.ToolStripMenuItem dateiToolStripMenuItem;
    private System.Windows.Forms.SaveFileDialog saveFileDialog1;
    private System.Windows.Forms.Panel panel1;
    private System.Windows.Forms.Label label7;
    private System.Windows.Forms.TextBox tbStatus;
    private System.Windows.Forms.Button btnConnect;
    private System.Windows.Forms.LinkLabel linkLabel1;
    private System.Windows.Forms.ComboBox cbRS232;
    private System.Windows.Forms.StatusStrip statusStrip1;
    private System.Windows.Forms.SplitContainer splitContainer1;
    private System.Windows.Forms.Panel panel2;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Panel panel3;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Button btnMessungStart;
    private System.Windows.Forms.Button btnMessungStop;
    private System.Windows.Forms.ToolStripStatusLabel tsLabelMessdauer;
    private System.Windows.Forms.ToolStripStatusLabel tsLabelMesswerte;
    private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel1;
    private System.Windows.Forms.ToolStripMenuItem programmBeendenToolStripMenuItem;
    private System.Windows.Forms.TextBox tbLVDT;
    private System.Windows.Forms.Label label9;
    private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel2;
    private System.Windows.Forms.ToolStripStatusLabel tsLabelMessdatei;
    private System.Windows.Forms.TextBox tbLVDTMaximum;
    private System.Windows.Forms.Label label13;
    private System.Windows.Forms.TextBox tbLVDTMinimum;
    private System.Windows.Forms.Label label11;
    private System.Windows.Forms.Panel panel4;
    private System.Windows.Forms.Label lbEinheitMaximum;
    private System.Windows.Forms.Label lbEinheitMinimum;
    private System.Windows.Forms.Label lbEinheitAktuellerMesswert;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.Label label15;
    private System.Windows.Forms.Label label16;
    private System.Windows.Forms.TextBox tbTemperaturMaximum;
    private System.Windows.Forms.Label label17;
    private System.Windows.Forms.TextBox tbTemperaturMinimum;
    private System.Windows.Forms.Label label18;
    private System.Windows.Forms.TextBox tbTemperatur;
    private System.Windows.Forms.Label label19;
    private System.Windows.Forms.CheckBox cbLVDT;
    private System.Windows.Forms.CheckBox cbTemperatur;
    private System.Windows.Forms.CheckBox cbPhasenverschiebung;
    private System.Windows.Forms.CheckBox cbMessungSpeichern;
    private System.Windows.Forms.Label lbEinheitOffset;
    private System.Windows.Forms.TextBox tbOffset;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.ToolStripStatusLabel tsLabelMessfrequenz;
    private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel3;
    private System.Windows.Forms.TextBox tbStatusregister;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.TextBox tbDDS;
    private System.Windows.Forms.Label label8;
    private System.Windows.Forms.TextBox tbAnzahlMittelwerte;
    private System.Windows.Forms.Label label23;
    private System.Windows.Forms.Label lbEinheitStdDev;
    private System.Windows.Forms.TextBox tbLVDTStdDev;
    private System.Windows.Forms.Label label25;
    private System.Windows.Forms.Label label27;
    private System.Windows.Forms.SplitContainer splitContainer2;
    private ZedGraph.ZedGraphControl zgc;
    private System.Windows.Forms.CheckBox cbGrafik;
    private System.Windows.Forms.Label label26;
    private System.Windows.Forms.Label label24;
    private System.Windows.Forms.Label lbEinheitAutoscaleYMax;
    private System.Windows.Forms.Label lbEinheitAutoscaleYMin;
    private System.Windows.Forms.TextBox tbY2Max;
    private System.Windows.Forms.Label label20;
    private System.Windows.Forms.TextBox tbY2Min;
    private System.Windows.Forms.Label label21;
    private System.Windows.Forms.CheckBox cbAutoscaleY2;
    private System.Windows.Forms.Label label14;
    private System.Windows.Forms.TextBox tbZeitausschnitt;
    private System.Windows.Forms.TextBox tbYMax;
    private System.Windows.Forms.Label label10;
    private System.Windows.Forms.TextBox tbYMin;
    private System.Windows.Forms.Label label12;
    private System.Windows.Forms.CheckBox cbXAusschnitt;
    private System.Windows.Forms.CheckBox cbAutoscaleY;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.TextBox tBmaxAbweichung;
        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.TextBox tBAusreisserZahl;
        private System.Windows.Forms.Label label30;
        private System.Windows.Forms.TextBox textBoxMaxAbw;
        private System.Windows.Forms.Label label29;
        private System.Windows.Forms.TextBox tbMesswertMM;
        private System.Windows.Forms.TextBox tbFaktorWeg;
        private System.Windows.Forms.Label label31;
        private System.Windows.Forms.ComboBox comboBoxLVDTtype;
        private System.Windows.Forms.Button buttonNullen;
    }
}

