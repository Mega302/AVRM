/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package mega_ascrm;
import Cris.*;
import gnu.io.*;
import gnu.io.SerialPort;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.*;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.sound.sampled.*;
import javax.swing.JOptionPane;
import mega_ascrm.TestPrinter.METRO_Printer;
/**
 *
 * @author root
 */
public class Ascrm extends javax.swing.JFrame implements Runnable 
{

    /**
     * Creates new form Ascrm
     */
        String[] portString = {"0", "1", "2", "3", "4", "5"};
        String[] totalNo_portString = {"1", "2", "3", "4", "5", "6"};
        public int fiveNotes,tenNotes,twentyNotes,fiftyNotes,hundredNotes,twohundredNotes,fiveHundredNotes,thousandNotes,twothousandNotes,totalNotes;
        public int fiveAmount,tenAmount,twentyAmount,fiftyAmount,hundredAmount,twohundredAmount,fiveHundredAmount,thousandAmount,twothousandAmount,totalAmount;
        int Time,denomTime;
        String sec;
        int Amount;
        Thread t1,timerThread;
        Currency CurrencyObj;
        SmartCard SmartCardObj;
        Security SecurityObj;
        public boolean flag;
        private volatile boolean threadflag = false;
        public long startingTime, endingTime, totalTime;
        int singleDenom=0;
        int rtValue = 0;        
        Toolkit tk;
        Dimension dim;
        File audioFile;
        AudioInputStream audioStream;
        AudioFormat format;
        Clip audioClip;
        SerialPort serialPortObj;
        InputStream inputStreamObj;
	OutputStream outputStreamObj;
    
        int escrowClearenceMode;
        int cardinitmode;
        int cardChannelClearanceMode;
        int cardReturnMode;
                
        public Ascrm() 
        {

                tk=Toolkit.getDefaultToolkit();
                dim=tk.getScreenSize();
                System.out.println("hight: "+dim.height+" width : "+dim.width);
                String audioFilePath = "30_Robots_.wav";
                audioFile = new File(audioFilePath);
                //portString[] = {"0", "1", "2", "3", "4", "5"};
                initComponents();
                flag=false;
                CurrencyObj=new Currency(1);
                cardReturnMode = 0;
                
                try
                {
                    SecurityObj = new Security(1);
                }
                catch(IllegalArgumentException ex)
                {
                    Logger.getLogger(Ascrm.class.getName()).log(Level.SEVERE, null, ex);
                }                
                
                try 
                {
                    SmartCardObj=new SmartCard(1);
                }// try  end
                catch (IllegalArgumentException ex) 
                {
                    Logger.getLogger(Ascrm.class.getName()).log(Level.SEVERE, null, ex);
                }// catch (IllegalArgumentException ex)  end
                catch (IOException ex) 
                {
                    Logger.getLogger(Ascrm.class.getName()).log(Level.SEVERE, null, ex);
                }// (IOException ex) end



                displayLog.addItemListener(new ItemListener() 
                {

                    @Override
                    public void itemStateChanged(ItemEvent e) 
                    {
                            if(e.getStateChange() == ItemEvent.SELECTED)
                            {                    
                                textLog.setVisible(true);
                                textLog.setEnabled(true);                                                
                            }
                            else if(e.getStateChange() == ItemEvent.DESELECTED)
                            {                        
                                textLog.setVisible(false);                       
                            }

                    }
                });{

            }
        }
    
    ///////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        public int GetNoteValue(int NoteCodeNumber)
        {
                switch(NoteCodeNumber)
                {
                                          case 0: 
                                                 return 5;                                                 
					      
                                          case 1:
                                                 return 10;                                                 

                                          case 2:
                                                 return 20;                                                 

                                          case 3:
                                                 return 50;                                                

                                          case 4:
                                                 return 100;
                                              
                                          case 5:
                                                 return 500;                                                

                                          case 6:
                                                 return 200;                                                
                                          
                                          case 7:
                                                 return 2000;                                     
                                                 
                                         default:
                                                 break;
                                                
                 };// switch(NoteCodeNumber) end
                 
		 return	0;	       
             
        }// public int GetNoteValue(int NoteCodeNumber) end
        
        @Override
        public void run()
        {
        
                textLog.insert("[void run()] Thread started\n",0);            
                
                fiveNotes=0;
                tenNotes=0;
                twentyNotes=0;
                fiftyNotes=0;
                hundredNotes=0;
		twohundredNotes=0;
                fiveHundredNotes=0;
                thousandNotes=0;
		twothousandNotes=0;
                totalNotes=0;

                fiveAmount=0;
                tenAmount=0;
                twentyAmount=0;
                fiftyAmount=0;
                hundredAmount=0;
                twohundredAmount=0;
                fiveHundredAmount=0;
                thousandAmount=0;
		twothousandAmount=0;
                totalAmount=0;


		int DenomArrayLength = 15;
                int previousAmount=0;
                byte[] acceptedNote1=new byte[DenomArrayLength];
                int userAmount=0;
                String amount;
                long startTime=0,endTime=0,totalTime=0;
                startTime = System.currentTimeMillis();
                
                int GetAcceptedAmount_rtcode=0;
 
                 while(true)
                 {
                        for(int i=0;i<DenomArrayLength;i++)
                        {
                               acceptedNote1[i] =-1;
                        }// for(int i=0;i<DenomArrayLength;i++) end
                        
                        GetAcceptedAmount_rtcode = CurrencyObj.GetAcceptedAmount(acceptedNote1);
                        userAmount =0 ;
                        
                        for(int i=0;i<DenomArrayLength;i++)
                        {
                                if(acceptedNote1[i]> -1 )
                                {
                                        userAmount+=GetNoteValue(acceptedNote1[i]);
                                        
                                }// if(acceptedNote1[i]>0) end
                              
                        }// for(int i=0;i<100;i++) end
                        
                        System.out.println("Current Amount"+userAmount);
                        System.out.println("Previous Amount"+previousAmount);
                        
                        if(userAmount != previousAmount)
                        {
                                    int diff=userAmount-previousAmount;
                                    previousAmount=userAmount;
                                    amount=Integer.toString(userAmount);
                                    textLog.insert( " User Input Amount: "+amount+"\n", 0);
                                    System.out.println(" [void run()] "+" User Input Amount: "+amount);
                                    totalAmount=userAmount;
                                    switch(diff)
                                    {

                                            case 5:     fiveNotes++;
                                                        fiveAmount=fiveNotes*5;
                                                        break;

                                            case 10:    tenNotes++;
                                                        tenAmount=tenNotes*10;
                                                        break;

                                            case 20:    twentyNotes++;
                                                        twentyAmount=twentyNotes*20;
                                                        break;

                                            case 50:    fiftyNotes++;
                                                        fiftyAmount=fiftyNotes*50;
                                                        break;

                                            case 100:   hundredNotes++;
                                                        hundredAmount=hundredNotes*100;
                                                        break;

					    case 200:   twohundredNotes++;
                                                        twohundredAmount=twohundredNotes*200;
                                                        break;

                                            case 500:   fiveHundredNotes++;
                                                        fiveHundredAmount=fiveHundredNotes*500;
                                                        break;

                                            case 1000:  thousandNotes++;
                                                        thousandAmount=thousandNotes*1000;
                                                        break;

   					    case 2000:  twothousandNotes++;
                                                        twothousandAmount=twothousandNotes*2000;
                                                        break;

                                    }// switch(diff) end                                                

                                    
                                    textLog.insert("Rs. 2000  X  "+twothousandNotes+" = "+twothousandAmount+"\n", 0);
                                    //textLog.insert("Rs. 1000  X  "+thousandNotes+" = "+thousandAmount+"\n", 0);
				    textLog.insert("Rs. 500  X  "+fiveHundredNotes+" = "+fiveHundredAmount+"\n", 0);
                                    
				    textLog.insert("Rs. 100  X  "+hundredNotes+" = "+hundredAmount+"\t\tRs. 200  X  "+twohundredNotes+" = "+twohundredAmount+"\n", 0);
                                    
				    textLog.insert("Rs. 20  X  "+twentyNotes+" = "+twentyAmount+"\t\tRs. 50  X  "+fiftyNotes+" = "+fiftyAmount+"\n", 0);
                                    textLog.insert("Rs. 5  X  "+fiveNotes+" = "+fiveAmount+"\t\tRs. 10  X  "+tenNotes+" = "+tenAmount+"\n", 0);                                                                                                                                                                                    
                                    textLog.insert("Total Notes = "+ totalNotes +"\n\n", 0);
                                    textLog.insert("Total Amount = Rs."+totalAmount+"\n\n", 0);
                                    textLog.insert("\n",0);
				    System.out.println("2000-"+twothousandNotes+"twhAMT-"+twothousandAmount);
                                    //System.out.println("1000-"+thousandNotes+"tAMT-"+thousandAmount);
                                    System.out.println("500-"+fiveHundredNotes+"fhAMT-"+fiveHundredAmount);
                                    System.out.println("200-"+twohundredNotes+"thAMT-"+twohundredAmount);
				    System.out.println("100-"+hundredNotes+"hAMT-"+hundredAmount);
                                    System.out.println("50-"+fiftyNotes+"fAMT-"+fiftyAmount);
                                    System.out.println("20-"+twentyNotes+"twAMT-"+twentyAmount);
                                    System.out.println("10-"+tenNotes+"teAMT-"+tenAmount);
                                    System.out.println("5-"+fiveNotes+"fiAMT-"+fiveAmount);
                            }// if(userAmount != previousAmount) end
                                
                            endTime   = System.currentTimeMillis();

                            totalTime =  (endTime - startTime );
                         
                            if( totalTime >=Time)
                            {
                                        flag=false;
                                        textLog.insert("Timeout\n", 0);
                                        threadflag = false;
                                        multiNoteTimeRem.setText("");
                                        break;
                            }// if( totalTime >=Time) end

                            if( totalAmount >=Amount )
                            {
                                        flag=false;
                                        //flagset();
                                        threadflag = false;
                                        multiNoteTimeRem.setText("");
                                        break;
                            }// if( totalAmount >=Amount ) end

                }// while(true) end
                multiNoteTimeRem.setText("");
        }// public void run() end

    

    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        cardManagement_ButtonGroup = new javax.swing.ButtonGroup();
        currencyManagement_ButtonGroup = new javax.swing.ButtonGroup();
        securityManagement_ButtonGroup = new javax.swing.ButtonGroup();
        primtermanagement = new javax.swing.JTabbedPane();
        cardmanagement = new javax.swing.JPanel();
        jLabel1 = new javax.swing.JLabel();
        connectCard = new javax.swing.JButton();
        keepCard = new javax.swing.JRadioButton();
        jLabel2 = new javax.swing.JLabel();
        stackCard = new javax.swing.JRadioButton();
        returnCard = new javax.swing.JRadioButton();
        jLabel3 = new javax.swing.JLabel();
        enable_cardEntry = new javax.swing.JButton();
        disable_cardEntry = new javax.swing.JButton();
        acceptCard = new javax.swing.JButton();
        jLabel4 = new javax.swing.JLabel();
        dispanseCard_movement = new javax.swing.JButton();
        rejectCard_movement = new javax.swing.JButton();
        returnCard_movement = new javax.swing.JButton();
        collectCard_movement = new javax.swing.JButton();
        jLabel5 = new javax.swing.JLabel();
        cardInChannel = new javax.swing.JButton();
        cardRemoved = new javax.swing.JButton();
        disconnectCard = new javax.swing.JButton();
        jLayeredPane1 = new javax.swing.JLayeredPane();
        jLabel29 = new javax.swing.JLabel();
        serialPortCard = new javax.swing.JComboBox();
        cardReader_InitButton = new javax.swing.JButton();
        initKeepCard = new javax.swing.JRadioButton();
        initCollectCard = new javax.swing.JRadioButton();
        initReturnCard = new javax.swing.JRadioButton();
        cardReaderStatusButton = new javax.swing.JButton();
        halfEjectCard = new javax.swing.JRadioButton();
        fullEjectCard = new javax.swing.JRadioButton();
        cardreadwrite = new javax.swing.JPanel();
        jLabel8 = new javax.swing.JLabel();
        contactlessCard_activate = new javax.swing.JButton();
        contactlessCard_read = new javax.swing.JButton();
        contactlessCard_deactivate = new javax.swing.JButton();
        contactlessCard_write = new javax.swing.JButton();
        read_des = new javax.swing.JButton();
        write_des = new javax.swing.JButton();
        contactlessCard_page = new javax.swing.JTextField();
        jLabel7 = new javax.swing.JLabel();
        jLabel9 = new javax.swing.JLabel();
        apduCmd = new javax.swing.JTextField();
        sendT_cl = new javax.swing.JButton();
        jLabel10 = new javax.swing.JLabel();
        contactCard_activate = new javax.swing.JButton();
        contactCard_poweron = new javax.swing.JButton();
        contactCard_deactivate = new javax.swing.JButton();
        contactCard_coldreset = new javax.swing.JButton();
        jTextField3 = new javax.swing.JTextField();
        send_t = new javax.swing.JButton();
        jLabel11 = new javax.swing.JLabel();
        contactCard_poweroff = new javax.swing.JButton();
        contactCard_wormreset = new javax.swing.JButton();
        jLabel35 = new javax.swing.JLabel();
        apduTime = new javax.swing.JTextField();
        isSAMEnable_button = new javax.swing.JButton();
        currencymanagement = new javax.swing.JPanel();
        jLabel12 = new javax.swing.JLabel();
        jLabel13 = new javax.swing.JLabel();
        currencyAccepter_connect = new javax.swing.JButton();
        currencyAccepter_keep = new javax.swing.JRadioButton();
        currencyAccepter_stack = new javax.swing.JRadioButton();
        currencyAccepter_return = new javax.swing.JRadioButton();
        jLabel14 = new javax.swing.JLabel();
        jLabel15 = new javax.swing.JLabel();
        singleNoteDenom = new javax.swing.JTextField();
        note_validate = new javax.swing.JButton();
        note_escrow = new javax.swing.JButton();
        note_return = new javax.swing.JButton();
        jLabel16 = new javax.swing.JLabel();
        enable_denoms = new javax.swing.JButton();
        accept_multinotes = new javax.swing.JButton();
        jLabel17 = new javax.swing.JLabel();
        jLabel18 = new javax.swing.JLabel();
        multiDenom_mask = new javax.swing.JTextField();
        multiAmount_rs = new javax.swing.JTextField();
        jLabel19 = new javax.swing.JLabel();
        stackNote_movement = new javax.swing.JButton();
        isNoteRemoved_movement = new javax.swing.JButton();
        returnNote_movement = new javax.swing.JButton();
        jLabel20 = new javax.swing.JLabel();
        disconnectCurrencyAccepter = new javax.swing.JButton();
        timeInSec = new javax.swing.JTextField();
        jLabel26 = new javax.swing.JLabel();
        noteAccepterStatusButton = new javax.swing.JButton();
        validateTime = new javax.swing.JLabel();
        stackTime = new javax.swing.JTextField();
        returnNoteTime = new javax.swing.JTextField();
        serialPort = new javax.swing.JComboBox();
        jLabel28 = new javax.swing.JLabel();
        jLabel31 = new javax.swing.JLabel();
        jLabel32 = new javax.swing.JLabel();
        multiNoteTimeRem = new javax.swing.JLabel();
        singleDenomTime = new javax.swing.JTextField();
        jLabel33 = new javax.swing.JLabel();
        default_Commit_Button = new javax.swing.JButton();
        default_Cancle_Button = new javax.swing.JButton();
        default_Commit_InputTime = new javax.swing.JTextField();
        jLabel6 = new javax.swing.JLabel();
        jLabel34 = new javax.swing.JLabel();
        default_Cancle_InputTime = new javax.swing.JTextField();
        securitymanagement = new javax.swing.JPanel();
        jLabel21 = new javax.swing.JLabel();
        jLabel22 = new javax.swing.JLabel();
        disableAlarm = new javax.swing.JButton();
        isMainDoorOpen = new javax.swing.JButton();
        isCashBoxOpen = new javax.swing.JButton();
        mainDoor_box = new javax.swing.JCheckBox();
        jLabel23 = new javax.swing.JLabel();
        mainDoor_duration = new javax.swing.JTextField();
        jLabel24 = new javax.swing.JLabel();
        cashBox_duration = new javax.swing.JTextField();
        jLabel25 = new javax.swing.JLabel();
        getUps_status = new javax.swing.JButton();
        getBattery_status = new javax.swing.JButton();
        onPowerSupply = new javax.swing.JRadioButton();
        onUpsBackup = new javax.swing.JRadioButton();
        jLabel27 = new javax.swing.JLabel();
        activateSecurityButton = new javax.swing.JButton();
        deactivateSecurityButton = new javax.swing.JButton();
        securityPort = new javax.swing.JComboBox();
        Printer = new javax.swing.JPanel();
        jScrollPane2 = new javax.swing.JScrollPane();
        printThis = new javax.swing.JTextArea();
        printButton = new javax.swing.JButton();
        printCups_button = new javax.swing.JButton();
        printerOnlineStatus_button = new javax.swing.JButton();
        jLabel30 = new javax.swing.JLabel();
        serialPortPrinter = new javax.swing.JComboBox();
        miscellaneousScreen = new javax.swing.JPanel();
        playAudioButton = new javax.swing.JButton();
        stopAudioButton = new javax.swing.JButton();
        test_Comports = new javax.swing.JButton();
        no_of_ports = new javax.swing.JComboBox();
        jLabel36 = new javax.swing.JLabel();
        timeout_ComPortTest = new javax.swing.JTextField();
        jLabel37 = new javax.swing.JLabel();
        jScrollPane1 = new javax.swing.JScrollPane();
        textLog = new javax.swing.JTextArea();
        displayLog = new javax.swing.JCheckBox();
        clearLog = new javax.swing.JButton();
        quitButton = new javax.swing.JButton();

        cardManagement_ButtonGroup.add(keepCard);
        cardManagement_ButtonGroup.add(stackCard);
        cardManagement_ButtonGroup.add(returnCard);

        currencyManagement_ButtonGroup.add(currencyAccepter_keep);
        currencyManagement_ButtonGroup.add(currencyAccepter_stack);
        currencyManagement_ButtonGroup.add(currencyAccepter_return);

        securityManagement_ButtonGroup.add(onPowerSupply);
        securityManagement_ButtonGroup.add(onUpsBackup);

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        primtermanagement.setTabLayoutPolicy(javax.swing.JTabbedPane.SCROLL_TAB_LAYOUT);
        primtermanagement.setPreferredSize(new java.awt.Dimension(1024, 768));

        cardmanagement.setPreferredSize(new java.awt.Dimension(1024, 768));

        jLabel1.setText("Smart Card Dispenser");

        connectCard.setText("Connect");
        connectCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                connectCardActionPerformed(evt);
            }
        });

        keepCard.setSelected(true);
        keepCard.setText("Keep Escrowed Card");
        keepCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                keepCardActionPerformed(evt);
            }
        });

        jLabel2.setText("Setup");

        stackCard.setText("Stack Escrowed Card");
        stackCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                stackCardActionPerformed(evt);
            }
        });

        returnCard.setText("Return Escrowed Card");
        returnCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                returnCardActionPerformed(evt);
            }
        });

        jLabel3.setText("Entry Control");

        enable_cardEntry.setText("Enable Card Entry");
        enable_cardEntry.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                enable_cardEntryActionPerformed(evt);
            }
        });

        disable_cardEntry.setText("Disable Card Entry");
        disable_cardEntry.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                disable_cardEntryActionPerformed(evt);
            }
        });

        acceptCard.setText("Accept Card");
        acceptCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                acceptCardActionPerformed(evt);
            }
        });

        jLabel4.setText("Card Movement");

        dispanseCard_movement.setText("Dispanse");
        dispanseCard_movement.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                dispanseCard_movementActionPerformed(evt);
            }
        });

        rejectCard_movement.setText("Reject");
        rejectCard_movement.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                rejectCard_movementActionPerformed(evt);
            }
        });

        returnCard_movement.setText("Return");
        returnCard_movement.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                returnCard_movementActionPerformed(evt);
            }
        });

        collectCard_movement.setText("Collect");
        collectCard_movement.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                collectCard_movementActionPerformed(evt);
            }
        });

        jLabel5.setText("Card Status");

        cardInChannel.setText("IsCardInChannel");
        cardInChannel.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cardInChannelActionPerformed(evt);
            }
        });

        cardRemoved.setText("IsCardRemoved");
        cardRemoved.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cardRemovedActionPerformed(evt);
            }
        });

        disconnectCard.setText("Disconnect");
        disconnectCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                disconnectCardActionPerformed(evt);
            }
        });

        jLabel29.setText("Serial Port No");

        serialPortCard.setModel(new javax.swing.DefaultComboBoxModel(portString));

        cardReader_InitButton.setText("Init");
        cardReader_InitButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cardReader_InitButtonActionPerformed(evt);
            }
        });

        initKeepCard.setText("Keep Card");
        initKeepCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                initKeepCardActionPerformed(evt);
            }
        });

        initCollectCard.setText("Collect Card");
        initCollectCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                initCollectCardActionPerformed(evt);
            }
        });

        initReturnCard.setText("Return Card");
        initReturnCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                initReturnCardActionPerformed(evt);
            }
        });

        cardReaderStatusButton.setText("Status");
        cardReaderStatusButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                cardReaderStatusButtonActionPerformed(evt);
            }
        });

        halfEjectCard.setSelected(true);
        halfEjectCard.setText("Hold");
        halfEjectCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                halfEjectCardActionPerformed(evt);
            }
        });

        fullEjectCard.setText("Throw");
        fullEjectCard.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                fullEjectCardActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout cardmanagementLayout = new javax.swing.GroupLayout(cardmanagement);
        cardmanagement.setLayout(cardmanagementLayout);
        cardmanagementLayout.setHorizontalGroup(
            cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(cardmanagementLayout.createSequentialGroup()
                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addComponent(cardInChannel, javax.swing.GroupLayout.PREFERRED_SIZE, 170, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(cardRemoved, javax.swing.GroupLayout.PREFERRED_SIZE, 180, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addComponent(dispanseCard_movement, javax.swing.GroupLayout.PREFERRED_SIZE, 170, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(returnCard_movement, javax.swing.GroupLayout.PREFERRED_SIZE, 180, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addGroup(cardmanagementLayout.createSequentialGroup()
                                .addGap(259, 259, 259)
                                .addComponent(jLayeredPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 0, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addGroup(cardmanagementLayout.createSequentialGroup()
                                .addGap(136, 136, 136)
                                .addComponent(jLabel1, javax.swing.GroupLayout.PREFERRED_SIZE, 145, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addGroup(cardmanagementLayout.createSequentialGroup()
                                .addGap(12, 12, 12)
                                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(jLabel2, javax.swing.GroupLayout.PREFERRED_SIZE, 50, javax.swing.GroupLayout.PREFERRED_SIZE)
                                    .addComponent(connectCard, javax.swing.GroupLayout.PREFERRED_SIZE, 110, javax.swing.GroupLayout.PREFERRED_SIZE))
                                .addGap(28, 28, 28)
                                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(jLabel29)
                                    .addComponent(serialPortCard, javax.swing.GroupLayout.PREFERRED_SIZE, 90, javax.swing.GroupLayout.PREFERRED_SIZE))
                                .addGap(60, 60, 60)
                                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(keepCard)
                                    .addComponent(stackCard)
                                    .addComponent(returnCard)))
                            .addGroup(cardmanagementLayout.createSequentialGroup()
                                .addGap(43, 43, 43)
                                .addComponent(disconnectCard, javax.swing.GroupLayout.PREFERRED_SIZE, 357, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addGroup(cardmanagementLayout.createSequentialGroup()
                                .addContainerGap()
                                .addComponent(cardReader_InitButton, javax.swing.GroupLayout.PREFERRED_SIZE, 71, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addGap(32, 32, 32)
                                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(initCollectCard)
                                    .addComponent(initKeepCard)
                                    .addComponent(initReturnCard))
                                .addGap(85, 85, 85)
                                .addComponent(cardReaderStatusButton, javax.swing.GroupLayout.PREFERRED_SIZE, 164, javax.swing.GroupLayout.PREFERRED_SIZE)))
                        .addGap(0, 6, Short.MAX_VALUE))
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, cardmanagementLayout.createSequentialGroup()
                        .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(enable_cardEntry, javax.swing.GroupLayout.PREFERRED_SIZE, 176, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(disable_cardEntry, javax.swing.GroupLayout.PREFERRED_SIZE, 176, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addGroup(cardmanagementLayout.createSequentialGroup()
                                .addContainerGap()
                                .addComponent(jLabel3, javax.swing.GroupLayout.PREFERRED_SIZE, 90, javax.swing.GroupLayout.PREFERRED_SIZE)))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(acceptCard, javax.swing.GroupLayout.PREFERRED_SIZE, 180, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addGap(12, 12, 12)
                        .addComponent(jLabel4, javax.swing.GroupLayout.PREFERRED_SIZE, 107, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(172, 172, 172)
                        .addComponent(halfEjectCard)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(fullEjectCard)))
                .addGap(12, 12, 12))
            .addGroup(cardmanagementLayout.createSequentialGroup()
                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jLabel5, javax.swing.GroupLayout.PREFERRED_SIZE, 130, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(0, 0, Short.MAX_VALUE))
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addComponent(rejectCard_movement, javax.swing.GroupLayout.PREFERRED_SIZE, 170, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(collectCard_movement, javax.swing.GroupLayout.PREFERRED_SIZE, 180, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap())
        );
        cardmanagementLayout.setVerticalGroup(
            cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(cardmanagementLayout.createSequentialGroup()
                .addComponent(jLayeredPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 0, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(6, 6, 6)
                .addComponent(jLabel1, javax.swing.GroupLayout.PREFERRED_SIZE, 23, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(11, 11, 11)
                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addGap(5, 5, 5)
                        .addComponent(jLabel2, javax.swing.GroupLayout.PREFERRED_SIZE, 20, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(6, 6, 6)
                        .addComponent(connectCard, javax.swing.GroupLayout.PREFERRED_SIZE, 33, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addComponent(jLabel29, javax.swing.GroupLayout.PREFERRED_SIZE, 20, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(10, 10, 10)
                        .addComponent(serialPortCard, javax.swing.GroupLayout.PREFERRED_SIZE, 30, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addGap(20, 20, 20)
                        .addComponent(keepCard)
                        .addGap(8, 8, 8)
                        .addComponent(stackCard)))
                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addGap(10, 10, 10)
                        .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(cardReader_InitButton)
                            .addComponent(initKeepCard))
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(initCollectCard)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(initReturnCard)
                        .addGap(39, 39, 39)
                        .addComponent(jLabel3, javax.swing.GroupLayout.PREFERRED_SIZE, 20, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(enable_cardEntry)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(disable_cardEntry))
                    .addGroup(cardmanagementLayout.createSequentialGroup()
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(returnCard)
                        .addGap(18, 18, 18)
                        .addComponent(cardReaderStatusButton)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(acceptCard, javax.swing.GroupLayout.PREFERRED_SIZE, 72, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addGap(18, 18, 18)
                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(jLabel4)
                    .addComponent(halfEjectCard)
                    .addComponent(fullEjectCard))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(dispanseCard_movement)
                    .addComponent(returnCard_movement))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(rejectCard_movement)
                    .addComponent(collectCard_movement))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jLabel5, javax.swing.GroupLayout.PREFERRED_SIZE, 23, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(cardmanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(cardInChannel)
                    .addComponent(cardRemoved))
                .addGap(18, 18, 18)
                .addComponent(disconnectCard))
        );

        primtermanagement.addTab("Card Management", cardmanagement);

        cardreadwrite.setPreferredSize(new java.awt.Dimension(1024, 768));
        cardreadwrite.setLayout(null);

        jLabel8.setText("Contactless Card");
        cardreadwrite.add(jLabel8);
        jLabel8.setBounds(150, 12, 124, 24);

        contactlessCard_activate.setText("Activate");
        contactlessCard_activate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                contactlessCard_activateActionPerformed(evt);
            }
        });
        cardreadwrite.add(contactlessCard_activate);
        contactlessCard_activate.setBounds(12, 89, 121, 29);

        contactlessCard_read.setText("Read UL Block");
        contactlessCard_read.setEnabled(false);
        cardreadwrite.add(contactlessCard_read);
        contactlessCard_read.setBounds(153, 89, 148, 29);

        contactlessCard_deactivate.setText("Deactivate");
        contactlessCard_deactivate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                contactlessCard_deactivateActionPerformed(evt);
            }
        });
        cardreadwrite.add(contactlessCard_deactivate);
        contactlessCard_deactivate.setBounds(12, 134, 121, 29);

        contactlessCard_write.setText("Write UL Block");
        contactlessCard_write.setEnabled(false);
        cardreadwrite.add(contactlessCard_write);
        contactlessCard_write.setBounds(151, 134, 148, 29);

        read_des.setText("Read DESFire EV1");
        read_des.setEnabled(false);
        read_des.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                read_desActionPerformed(evt);
            }
        });
        cardreadwrite.add(read_des);
        read_des.setBounds(12, 207, 164, 29);

        write_des.setText("Write DESFire EV1");
        write_des.setEnabled(false);
        cardreadwrite.add(write_des);
        write_des.setBounds(290, 207, 181, 29);

        contactlessCard_page.setEnabled(false);
        cardreadwrite.add(contactlessCard_page);
        contactlessCard_page.setBounds(386, 130, 50, 37);

        jLabel7.setText("Contactless Card Operations");
        cardreadwrite.add(jLabel7);
        jLabel7.setBounds(12, 54, 194, 23);

        jLabel9.setText("Page NO");
        cardreadwrite.add(jLabel9);
        jLabel9.setBounds(380, 91, 56, 25);
        cardreadwrite.add(apduCmd);
        apduCmd.setBounds(12, 254, 459, 27);

        sendT_cl.setText("Send T = CL APDU");
        sendT_cl.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                sendT_clActionPerformed(evt);
            }
        });
        cardreadwrite.add(sendT_cl);
        sendT_cl.setBounds(111, 293, 360, 29);

        jLabel10.setText("Contact Card");
        cardreadwrite.add(jLabel10);
        jLabel10.setBounds(212, 354, 105, 25);

        contactCard_activate.setText("Activate");
        contactCard_activate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                contactCard_activateActionPerformed(evt);
            }
        });
        cardreadwrite.add(contactCard_activate);
        contactCard_activate.setBounds(12, 431, 112, 29);

        contactCard_poweron.setText("PowerOn");
        contactCard_poweron.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                contactCard_poweronActionPerformed(evt);
            }
        });
        cardreadwrite.add(contactCard_poweron);
        contactCard_poweron.setBounds(156, 431, 161, 29);

        contactCard_deactivate.setText("Deactivate");
        contactCard_deactivate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                contactCard_deactivateActionPerformed(evt);
            }
        });
        cardreadwrite.add(contactCard_deactivate);
        contactCard_deactivate.setBounds(12, 478, 112, 29);

        contactCard_coldreset.setText("Cold Reset");
        contactCard_coldreset.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                contactCard_coldresetActionPerformed(evt);
            }
        });
        cardreadwrite.add(contactCard_coldreset);
        contactCard_coldreset.setBounds(355, 478, 116, 29);

        jTextField3.setEnabled(false);
        cardreadwrite.add(jTextField3);
        jTextField3.setBounds(12, 525, 459, 28);

        send_t.setText("Send T = 1 APDU");
        send_t.setEnabled(false);
        send_t.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                send_tActionPerformed(evt);
            }
        });
        cardreadwrite.add(send_t);
        send_t.setBounds(12, 565, 459, 29);

        jLabel11.setText("Contact Card Operations");
        cardreadwrite.add(jLabel11);
        jLabel11.setBounds(12, 397, 178, 22);

        contactCard_poweroff.setText("PowerOff");
        contactCard_poweroff.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                contactCard_poweroffActionPerformed(evt);
            }
        });
        cardreadwrite.add(contactCard_poweroff);
        contactCard_poweroff.setBounds(156, 478, 161, 29);

        contactCard_wormreset.setText("Worm Reset");
        contactCard_wormreset.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                contactCard_wormresetActionPerformed(evt);
            }
        });
        cardreadwrite.add(contactCard_wormreset);
        contactCard_wormreset.setBounds(355, 431, 116, 29);

        jLabel35.setText("Time");
        cardreadwrite.add(jLabel35);
        jLabel35.setBounds(10, 290, 40, 30);
        cardreadwrite.add(apduTime);
        apduTime.setBounds(60, 290, 40, 27);
        
        isSAMEnable_button.setText("Is SAM Enable");
        isSAMEnable_button.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                isSAMEnable_buttonActionPerformed(evt);
            }
        });
        cardreadwrite.add(isSAMEnable_button);
        isSAMEnable_button.setBounds(270, 390, 130, 29);
        

        primtermanagement.addTab("Card Read Write", cardreadwrite);

        currencymanagement.setPreferredSize(new java.awt.Dimension(1024, 768));
        currencymanagement.setLayout(null);

        jLabel12.setText("Currency Accepter");
        currencymanagement.add(jLabel12);
        jLabel12.setBounds(150, 0, 123, 29);

        jLabel13.setText("Connect");
        currencymanagement.add(jLabel13);
        jLabel13.setBounds(10, 30, 59, 23);

        currencyAccepter_connect.setText("Connect");
        currencyAccepter_connect.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                currencyAccepter_connectActionPerformed(evt);
            }
        });
        currencymanagement.add(currencyAccepter_connect);
        currencyAccepter_connect.setBounds(10, 60, 100, 40);

        currencyAccepter_keep.setSelected(true);
        currencyAccepter_keep.setText("Keep Escrowed Note");
        currencyAccepter_keep.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                currencyAccepter_keepActionPerformed(evt);
            }
        });
        currencymanagement.add(currencyAccepter_keep);
        currencyAccepter_keep.setBounds(308, 26, 155, 22);

        currencyAccepter_stack.setText("Stack Escrowed Note");
        currencyAccepter_stack.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                currencyAccepter_stackActionPerformed(evt);
            }
        });
        currencymanagement.add(currencyAccepter_stack);
        currencyAccepter_stack.setBounds(308, 54, 159, 22);

        currencyAccepter_return.setText("Return Escrowed Note");
        currencyAccepter_return.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                currencyAccepter_returnActionPerformed(evt);
            }
        });
        currencymanagement.add(currencyAccepter_return);
        currencyAccepter_return.setBounds(308, 82, 165, 22);

        jLabel14.setText("Single Note");
        currencymanagement.add(jLabel14);
        jLabel14.setBounds(10, 160, 80, 25);

        jLabel15.setText("Denom");
        currencymanagement.add(jLabel15);
        jLabel15.setBounds(10, 190, 47, 17);
        currencymanagement.add(singleNoteDenom);
        singleNoteDenom.setBounds(10, 220, 45, 31);

        note_validate.setText("Validate");
        note_validate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                note_validateActionPerformed(evt);
            }
        });
        currencymanagement.add(note_validate);
        note_validate.setBounds(190, 220, 80, 29);

        note_escrow.setText("Escrow");
        note_escrow.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                note_escrowActionPerformed(evt);
            }
        });
        currencymanagement.add(note_escrow);
        note_escrow.setBounds(280, 220, 89, 29);

        note_return.setText("Return");
        note_return.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                note_returnActionPerformed(evt);
            }
        });
        currencymanagement.add(note_return);
        note_return.setBounds(380, 220, 89, 29);

        jLabel16.setText("Multi Notes");
        currencymanagement.add(jLabel16);
        jLabel16.setBounds(12, 277, 81, 23);

        enable_denoms.setText("Enable Denoms");
        enable_denoms.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                enable_denomsActionPerformed(evt);
            }
        });
        currencymanagement.add(enable_denoms);
        enable_denoms.setBounds(10, 310, 130, 29);

        accept_multinotes.setText("Accept Notes");
        accept_multinotes.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                accept_multinotesActionPerformed(evt);
            }
        });
        currencymanagement.add(accept_multinotes);
        accept_multinotes.setBounds(10, 350, 130, 29);

        jLabel17.setText("Denom Mask");
        currencymanagement.add(jLabel17);
        jLabel17.setBounds(236, 309, 104, 17);

        jLabel18.setText("Amount Rs.");
        currencymanagement.add(jLabel18);
        jLabel18.setBounds(236, 342, 104, 17);
        currencymanagement.add(multiDenom_mask);
        multiDenom_mask.setBounds(374, 305, 89, 25);
        currencymanagement.add(multiAmount_rs);
        multiAmount_rs.setBounds(374, 338, 89, 25);

        jLabel19.setText("Note Movement");
        currencymanagement.add(jLabel19);
        jLabel19.setBounds(10, 420, 107, 23);

        stackNote_movement.setText("Stack Notes");
        stackNote_movement.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                stackNote_movementActionPerformed(evt);
            }
        });
        currencymanagement.add(stackNote_movement);
        stackNote_movement.setBounds(10, 450, 158, 29);

        isNoteRemoved_movement.setText("IsNoteRemoved");
        isNoteRemoved_movement.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                isNoteRemoved_movementActionPerformed(evt);
            }
        });
        currencymanagement.add(isNoteRemoved_movement);
        isNoteRemoved_movement.setBounds(10, 490, 158, 29);

        returnNote_movement.setText("Return Note");
        returnNote_movement.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                returnNote_movementActionPerformed(evt);
            }
        });
        currencymanagement.add(returnNote_movement);
        returnNote_movement.setBounds(280, 450, 148, 29);

        jLabel20.setText("Disconnect");
        currencymanagement.add(jLabel20);
        jLabel20.setBounds(12, 525, 83, 23);

        disconnectCurrencyAccepter.setText("Disconnect");
        disconnectCurrencyAccepter.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                disconnectCurrencyAccepterActionPerformed(evt);
            }
        });
        currencymanagement.add(disconnectCurrencyAccepter);
        disconnectCurrencyAccepter.setBounds(45, 560, 355, 29);
        currencymanagement.add(timeInSec);
        timeInSec.setBounds(374, 373, 89, 27);

        jLabel26.setText("Time in sec");
        currencymanagement.add(jLabel26);
        jLabel26.setBounds(236, 378, 87, 17);

        noteAccepterStatusButton.setText("Status");
        noteAccepterStatusButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                noteAccepterStatusButtonActionPerformed(evt);
            }
        });
        currencymanagement.add(noteAccepterStatusButton);
        noteAccepterStatusButton.setBounds(10, 110, 100, 29);
        currencymanagement.add(validateTime);
        validateTime.setBounds(190, 260, 80, 20);
        currencymanagement.add(stackTime);
        stackTime.setBounds(180, 450, 30, 27);
        currencymanagement.add(returnNoteTime);
        returnNoteTime.setBounds(440, 450, 30, 27);

        serialPort.setModel(new javax.swing.DefaultComboBoxModel(portString));
        currencymanagement.add(serialPort);
        serialPort.setBounds(140, 60, 90, 40);

        jLabel28.setText("Serial Port No");
        currencymanagement.add(jLabel28);
        jLabel28.setBounds(140, 30, 88, 20);

        jLabel31.setText("Time Rem");
        currencymanagement.add(jLabel31);
        jLabel31.setBounds(240, 410, 70, 17);

        jLabel32.setText("Time Rem");
        currencymanagement.add(jLabel32);
        jLabel32.setBounds(100, 260, 70, 17);
        currencymanagement.add(multiNoteTimeRem);
        multiNoteTimeRem.setBounds(380, 407, 80, 20);
        currencymanagement.add(singleDenomTime);
        singleDenomTime.setBounds(80, 220, 40, 30);

        jLabel33.setText("Time");
        currencymanagement.add(jLabel33);
        jLabel33.setBounds(80, 190, 53, 17);

        default_Commit_Button.setText("Default Commit");
        default_Commit_Button.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                default_Commit_ButtonActionPerformed(evt);
            }
        });
        currencymanagement.add(default_Commit_Button);
        default_Commit_Button.setBounds(180, 130, 150, 29);

        default_Cancle_Button.setText("Default Cancle");
        default_Cancle_Button.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                default_Cancle_ButtonActionPerformed(evt);
            }
        });
        currencymanagement.add(default_Cancle_Button);
        default_Cancle_Button.setBounds(180, 170, 150, 29);
        currencymanagement.add(default_Commit_InputTime);
        default_Commit_InputTime.setBounds(410, 130, 40, 27);

        jLabel6.setText("Time");
        currencymanagement.add(jLabel6);
        jLabel6.setBounds(350, 127, 45, 30);

        jLabel34.setText("Time");
        currencymanagement.add(jLabel34);
        jLabel34.setBounds(350, 170, 40, 30);
        currencymanagement.add(default_Cancle_InputTime);
        default_Cancle_InputTime.setBounds(407, 170, 40, 27);

        primtermanagement.addTab("Currency Management", currencymanagement);

        securitymanagement.setMinimumSize(new java.awt.Dimension(1024, 768));

        jLabel21.setText("Security Mechanism & UPS");

        jLabel22.setText("Disable Alarm");

        disableAlarm.setText("Disable Alarm");
        disableAlarm.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                disableAlarmActionPerformed(evt);
            }
        });

        isMainDoorOpen.setText("IsMainDoorOpen?");
        isMainDoorOpen.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                isMainDoorOpenActionPerformed(evt);
            }
        });

        isCashBoxOpen.setText("IsCashBoxOpen?");
        isCashBoxOpen.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                isCashBoxOpenActionPerformed(evt);
            }
        });

        mainDoor_box.setText("Main Door");
        mainDoor_box.setEnabled(false);

        jLabel23.setText("Duration ");

        mainDoor_duration.setEnabled(false);

        jLabel24.setText("Duration");

        jLabel25.setText("UPS Status");

        getUps_status.setText("Get UPS Status");
        getUps_status.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                getUps_statusActionPerformed(evt);
            }
        });

        getBattery_status.setText("Get Battery Status");
        getBattery_status.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                getBattery_statusActionPerformed(evt);
            }
        });

        onPowerSupply.setText("On Power Supply");
        onPowerSupply.setEnabled(false);

        onUpsBackup.setText("On UPS Backup");
        onUpsBackup.setEnabled(false);

        jLabel27.setText("Cash Box");

        activateSecurityButton.setText("Activate Security");
        activateSecurityButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                activateSecurityButtonActionPerformed(evt);
            }
        });

        deactivateSecurityButton.setText("Deactivate Security");
        deactivateSecurityButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                deactivateSecurityButtonActionPerformed(evt);
            }
        });

        securityPort.setModel(new javax.swing.DefaultComboBoxModel(portString));

        javax.swing.GroupLayout securitymanagementLayout = new javax.swing.GroupLayout(securitymanagement);
        securitymanagement.setLayout(securitymanagementLayout);
        securitymanagementLayout.setHorizontalGroup(
            securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(securitymanagementLayout.createSequentialGroup()
                .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(securitymanagementLayout.createSequentialGroup()
                        .addGap(140, 140, 140)
                        .addComponent(jLabel21, javax.swing.GroupLayout.PREFERRED_SIZE, 190, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(securitymanagementLayout.createSequentialGroup()
                        .addGap(10, 10, 10)
                        .addComponent(jLabel22, javax.swing.GroupLayout.PREFERRED_SIZE, 96, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(securitymanagementLayout.createSequentialGroup()
                        .addGap(20, 20, 20)
                        .addComponent(isMainDoorOpen, javax.swing.GroupLayout.PREFERRED_SIZE, 180, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(70, 70, 70)
                        .addComponent(isCashBoxOpen, javax.swing.GroupLayout.PREFERRED_SIZE, 190, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(securitymanagementLayout.createSequentialGroup()
                        .addGap(20, 20, 20)
                        .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                            .addComponent(getBattery_status, javax.swing.GroupLayout.PREFERRED_SIZE, 150, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(getUps_status, javax.swing.GroupLayout.PREFERRED_SIZE, 150, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(130, 130, 130)
                        .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(onUpsBackup, javax.swing.GroupLayout.PREFERRED_SIZE, 140, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(onPowerSupply, javax.swing.GroupLayout.PREFERRED_SIZE, 150, javax.swing.GroupLayout.PREFERRED_SIZE)))
                    .addGroup(securitymanagementLayout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jLabel25, javax.swing.GroupLayout.PREFERRED_SIZE, 80, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(securitymanagementLayout.createSequentialGroup()
                        .addGap(29, 29, 29)
                        .addComponent(deactivateSecurityButton, javax.swing.GroupLayout.PREFERRED_SIZE, 410, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(securitymanagementLayout.createSequentialGroup()
                        .addGap(20, 20, 20)
                        .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                            .addGroup(securitymanagementLayout.createSequentialGroup()
                                .addComponent(activateSecurityButton, javax.swing.GroupLayout.PREFERRED_SIZE, 284, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                                .addComponent(securityPort, javax.swing.GroupLayout.PREFERRED_SIZE, 91, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addGroup(securitymanagementLayout.createSequentialGroup()
                                .addComponent(disableAlarm, javax.swing.GroupLayout.PREFERRED_SIZE, 120, javax.swing.GroupLayout.PREFERRED_SIZE)
                                .addGap(30, 30, 30)
                                .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(mainDoor_box)
                                    .addGroup(securitymanagementLayout.createSequentialGroup()
                                        .addGap(10, 10, 10)
                                        .addComponent(jLabel27, javax.swing.GroupLayout.PREFERRED_SIZE, 90, javax.swing.GroupLayout.PREFERRED_SIZE)))
                                .addGap(50, 50, 50)
                                .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(jLabel23)
                                    .addComponent(jLabel24, javax.swing.GroupLayout.PREFERRED_SIZE, 64, javax.swing.GroupLayout.PREFERRED_SIZE))
                                .addGap(26, 26, 26)
                                .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                                    .addComponent(mainDoor_duration, javax.swing.GroupLayout.PREFERRED_SIZE, 40, javax.swing.GroupLayout.PREFERRED_SIZE)
                                    .addComponent(cashBox_duration, javax.swing.GroupLayout.PREFERRED_SIZE, 40, javax.swing.GroupLayout.PREFERRED_SIZE))))))
                .addGap(564, 564, 564))
        );
        securitymanagementLayout.setVerticalGroup(
            securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(securitymanagementLayout.createSequentialGroup()
                .addGap(40, 40, 40)
                .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(activateSecurityButton)
                    .addComponent(securityPort, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(21, 21, 21)
                .addComponent(jLabel21, javax.swing.GroupLayout.PREFERRED_SIZE, 28, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(12, 12, 12)
                .addComponent(jLabel22, javax.swing.GroupLayout.PREFERRED_SIZE, 24, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(6, 6, 6)
                .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(securitymanagementLayout.createSequentialGroup()
                        .addComponent(mainDoor_duration, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(23, 23, 23)
                        .addComponent(cashBox_duration, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(securitymanagementLayout.createSequentialGroup()
                        .addGap(10, 10, 10)
                        .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(disableAlarm, javax.swing.GroupLayout.PREFERRED_SIZE, 70, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addGroup(securitymanagementLayout.createSequentialGroup()
                                .addComponent(mainDoor_box)
                                .addGap(28, 28, 28)
                                .addComponent(jLabel27, javax.swing.GroupLayout.PREFERRED_SIZE, 20, javax.swing.GroupLayout.PREFERRED_SIZE))
                            .addGroup(securitymanagementLayout.createSequentialGroup()
                                .addComponent(jLabel23)
                                .addGap(33, 33, 33)
                                .addComponent(jLabel24)))))
                .addGap(40, 40, 40)
                .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(isMainDoorOpen)
                    .addComponent(isCashBoxOpen))
                .addGap(26, 26, 26)
                .addComponent(deactivateSecurityButton)
                .addGap(38, 38, 38)
                .addComponent(jLabel25, javax.swing.GroupLayout.PREFERRED_SIZE, 24, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(getUps_status)
                    .addComponent(onPowerSupply))
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addGroup(securitymanagementLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(getBattery_status)
                    .addComponent(onUpsBackup))
                .addGap(254, 254, 254))
        );

        primtermanagement.addTab("Security Management", securitymanagement);

        Printer.setPreferredSize(new java.awt.Dimension(1024, 768));
        Printer.setLayout(null);

        printThis.setColumns(20);
        printThis.setRows(5);
        jScrollPane2.setViewportView(printThis);

        Printer.add(jScrollPane2);
        jScrollPane2.setBounds(12, 218, 459, 220);

        printButton.setText("Print");
        printButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                printButtonActionPerformed(evt);
            }
        });
        Printer.add(printButton);
        printButton.setBounds(10, 460, 459, 40);

        printCups_button.setText("Cups Print");
        Printer.add(printCups_button);
        printCups_button.setBounds(70, 140, 230, 50);

        printerOnlineStatus_button.setText("Printer Online Status");
        printerOnlineStatus_button.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                printerOnlineStatus_buttonActionPerformed(evt);
            }
        });
        Printer.add(printerOnlineStatus_button);
        printerOnlineStatus_button.setBounds(70, 70, 230, 52);

        jLabel30.setText("Serial Port No");
        Printer.add(jLabel30);
        jLabel30.setBounds(340, 70, 88, 20);

        serialPortPrinter.setModel(new javax.swing.DefaultComboBoxModel(portString));
        Printer.add(serialPortPrinter);
        serialPortPrinter.setBounds(340, 100, 90, 40);

        primtermanagement.addTab("Printer Management", Printer);

        playAudioButton.setText("Play Audio");
        playAudioButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                playAudioButtonActionPerformed(evt);
            }
        });

        stopAudioButton.setText("Stop Audio");
        stopAudioButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                stopAudioButtonActionPerformed(evt);
            }
        });

        test_Comports.setText("Test COM Ports");
        test_Comports.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                test_ComportsActionPerformed(evt);
            }
        });

        no_of_ports.setModel(new javax.swing.DefaultComboBoxModel(totalNo_portString));
        no_of_ports.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                no_of_portsActionPerformed(evt);
            }
        });

        jLabel36.setText("Total Number of Ports ");

        jLabel37.setText("TimeOut in seconds : ");

        javax.swing.GroupLayout miscellaneousScreenLayout = new javax.swing.GroupLayout(miscellaneousScreen);
        miscellaneousScreen.setLayout(miscellaneousScreenLayout);
        miscellaneousScreenLayout.setHorizontalGroup(
            miscellaneousScreenLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(miscellaneousScreenLayout.createSequentialGroup()
                .addGap(65, 65, 65)
                .addGroup(miscellaneousScreenLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(miscellaneousScreenLayout.createSequentialGroup()
                        .addComponent(playAudioButton, javax.swing.GroupLayout.PREFERRED_SIZE, 126, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(stopAudioButton, javax.swing.GroupLayout.PREFERRED_SIZE, 117, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(70, 70, 70))
                    .addGroup(miscellaneousScreenLayout.createSequentialGroup()
                        .addComponent(test_Comports, javax.swing.GroupLayout.PREFERRED_SIZE, 162, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(80, 80, 80)
                        .addGroup(miscellaneousScreenLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jLabel36)
                            .addComponent(no_of_ports, javax.swing.GroupLayout.PREFERRED_SIZE, 90, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(jLabel37)
                            .addComponent(timeout_ComPortTest, javax.swing.GroupLayout.PREFERRED_SIZE, 120, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addContainerGap(32, Short.MAX_VALUE))))
        );
        miscellaneousScreenLayout.setVerticalGroup(
            miscellaneousScreenLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(miscellaneousScreenLayout.createSequentialGroup()
                .addGap(141, 141, 141)
                .addGroup(miscellaneousScreenLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                    .addComponent(playAudioButton, javax.swing.GroupLayout.PREFERRED_SIZE, 55, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(stopAudioButton, javax.swing.GroupLayout.PREFERRED_SIZE, 55, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(18, 18, 18)
                .addComponent(jLabel37)
                .addGap(5, 5, 5)
                .addComponent(timeout_ComPortTest, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(miscellaneousScreenLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(test_Comports, javax.swing.GroupLayout.PREFERRED_SIZE, 69, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(miscellaneousScreenLayout.createSequentialGroup()
                        .addComponent(jLabel36)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(no_of_ports, javax.swing.GroupLayout.PREFERRED_SIZE, 40, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap(292, Short.MAX_VALUE))
        );

        primtermanagement.addTab("Miscellaneous", miscellaneousScreen);

        textLog.setColumns(20);
        textLog.setEditable(false);
        textLog.setLineWrap(true);
        textLog.setRows(30);
        textLog.setWrapStyleWord(true);
        jScrollPane1.setViewportView(textLog);

        displayLog.setSelected(true);
        displayLog.setText("Display Log");
        displayLog.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                displayLogActionPerformed(evt);
            }
        });

        clearLog.setText("Clear Log");
        clearLog.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                clearLogActionPerformed(evt);
            }
        });

        quitButton.setText("Quit");
        quitButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                quitButtonActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(primtermanagement, javax.swing.GroupLayout.PREFERRED_SIZE, 495, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                            .addComponent(jScrollPane1, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 702, Short.MAX_VALUE)
                            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                                .addComponent(displayLog)
                                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 480, Short.MAX_VALUE)
                                .addComponent(clearLog, javax.swing.GroupLayout.PREFERRED_SIZE, 117, javax.swing.GroupLayout.PREFERRED_SIZE)))
                        .addContainerGap())
                    .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                        .addComponent(quitButton, javax.swing.GroupLayout.PREFERRED_SIZE, 78, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addGap(295, 295, 295))))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                        .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                            .addComponent(clearLog, javax.swing.GroupLayout.PREFERRED_SIZE, 33, javax.swing.GroupLayout.PREFERRED_SIZE)
                            .addComponent(displayLog, javax.swing.GroupLayout.PREFERRED_SIZE, 26, javax.swing.GroupLayout.PREFERRED_SIZE))
                        .addGap(10, 10, 10)
                        .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 583, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(quitButton, javax.swing.GroupLayout.PREFERRED_SIZE, 35, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addComponent(primtermanagement, javax.swing.GroupLayout.PREFERRED_SIZE, 673, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap(190, Short.MAX_VALUE))
        );

        java.awt.Dimension screenSize = java.awt.Toolkit.getDefaultToolkit().getScreenSize();
        setBounds(0, 0, dim.width, dim.height);
    }// </editor-fold>//GEN-END:initComponents
        public void EnableAtInitTime()
        {

                currencyAccepter_connect.setEnabled(false);
                currencyAccepter_keep.setEnabled(false);
                currencyAccepter_stack.setEnabled(false);
                currencyAccepter_return.setEnabled(false);
                singleNoteDenom.setEnabled(true);
                note_validate.setEnabled(true);
                note_escrow.setEnabled(true);
                note_return.setEnabled(true);
                enable_denoms.setEnabled(true);
                noteAccepterStatusButton.setEnabled(true);
                //multiDenom_mask.setEnabled(true);
                accept_multinotes.setEnabled(true);
                multiAmount_rs.setEnabled(true);
                timeInSec.setEnabled(true);
                stackNote_movement.setEnabled(true);
                isNoteRemoved_movement.setEnabled(true);
                returnNote_movement.setEnabled(true);
                disconnectCurrencyAccepter.setEnabled(true);
        
   
        }// public void EnableAtInitTime() end
   
        public void DisableAtInitTime()
        {
   
                currencyAccepter_connect.setEnabled(true);
                currencyAccepter_keep.setEnabled(true);
                currencyAccepter_stack.setEnabled(true);
                currencyAccepter_return.setEnabled(true);
                enable_denoms.setEnabled(false);
                //multiDenom_mask.setEnabled(false);
                accept_multinotes.setEnabled(false);
                multiAmount_rs.setEnabled(false);
                timeInSec.setEnabled(false);
                stackNote_movement.setEnabled(false);
                isNoteRemoved_movement.setEnabled(false);
                returnNote_movement.setEnabled(false);
                disconnectCurrencyAccepter.setEnabled(false);
       
        }// public void DisableAtInitTime() end
   
    private void quitButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_quitButtonActionPerformed
 
            System.exit(0);                                                             //Exit        
        // TODO add your handling code here:
    }//GEN-LAST:event_quitButtonActionPerformed

    private void displayLogActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_displayLogActionPerformed
                //{
               // textLog.setEnabled(false);
                //}            
    }//GEN-LAST:event_displayLogActionPerformed

    private void clearLogActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_clearLogActionPerformed

            textLog.setText(null);                                                     //Cleaning Display Log                
    }//GEN-LAST:event_clearLogActionPerformed

    private void stopAudioButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_stopAudioButtonActionPerformed

        audioClip.close();
        audioClip.stop();
    }//GEN-LAST:event_stopAudioButtonActionPerformed

    private void playAudioButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_playAudioButtonActionPerformed

        try {
            audioStream = AudioSystem.getAudioInputStream(audioFile);
        } catch (UnsupportedAudioFileException ex) {
            System.out.println(" [ playAudioButtonActionPerformed() ] UnsupportedAudioFileException in getAudioInputStream()");
        } catch (IOException ex) {
            System.out.println(" [ playAudioButtonActionPerformed() ] IOException in getAudioInputStream()");
        }

        AudioFormat format = audioStream.getFormat();
        System.out.println(format);
        DataLine.Info info = new DataLine.Info(Clip.class, format);

        try {
            audioClip = (Clip) AudioSystem.getLine(info);
        } catch (LineUnavailableException ex) {
            System.out.println(" [ playAudioButtonActionPerformed() ] LineUnavailableException in getLine() ");
        }

        try {
            audioClip.open(audioStream);
        } catch (LineUnavailableException ex) {
            System.out.println(" [ playAudioButtonActionPerformed() ] LineUnavailableException in open() ");
        } catch (IOException ex) {
            System.out.println(" [ playAudioButtonActionPerformed() ] IOException in open() ");
        }// catch (IOException ex) end

        audioClip.start();

    }//GEN-LAST:event_playAudioButtonActionPerformed

    private void printerOnlineStatus_buttonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_printerOnlineStatus_buttonActionPerformed

        textLog.insert("\n", 0);

        METRO_Printer mp = new METRO_Printer();
        byte[] recv_byte = new byte[100];
        String printthis = printThis.getText();
        System.out.println("/dev/ttyS" + String.valueOf(serialPortPrinter.getSelectedItem()));
        mp.connectPort("/dev/ttyS" + String.valueOf(serialPortPrinter.getSelectedItem()));

        mp.printer_Online_Status();
        mp.Read_Reply(recv_byte, 7);
        for (int i = 0; i < 7; i++) {
            textLog.insert("\n Printer Online Status :" + " [ " + i + " ] " + Integer.toHexString(recv_byte[i]), 0);
        }

        if (false == Integer.toHexString(0).equals(Integer.toHexString(recv_byte[2])) || false == Integer.toHexString(0).equals(Integer.toHexString(recv_byte[3]))) {
            JOptionPane.showMessageDialog(null, "Printer Not Configured Properly", "Printer Management", JOptionPane.WARNING_MESSAGE);
        }// if( Integer.toHexString(0)==Integer.toHexString(recv_byte[2]) && Integer.toHexString(0)==Integer.toHexString(recv_byte[3])) end

        mp.disconnectPort();
    }//GEN-LAST:event_printerOnlineStatus_buttonActionPerformed

    private void printButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_printButtonActionPerformed

        textLog.insert("\n\n", 0);
        byte[] recv_byte = new byte[100];
        METRO_Printer mp = new METRO_Printer();
        String printthis = printThis.getText();
        mp.connectPort("/dev/ttyS" + String.valueOf(serialPortPrinter.getSelectedItem()));
        mp.writePort("\n" + printthis);
        mp.printer_Online_Status();
        mp.Read_Reply(recv_byte, 7);
        for (int i = 0; i < 7; i++) {
            textLog.insert("\n Printer Online Status :" + " [ " + i + " ] " + Integer.toHexString(recv_byte[i]), 0);
        }

        mp.disconnectPort();
    }//GEN-LAST:event_printButtonActionPerformed

    private void deactivateSecurityButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_deactivateSecurityButtonActionPerformed

        int returnValue = 0;
        returnValue = SecurityObj.DeActivateSecurityPort();
        textLog.insert("\n Security Deactivation Return Code. " + returnValue, 0);

    }//GEN-LAST:event_deactivateSecurityButtonActionPerformed

    private void activateSecurityButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_activateSecurityButtonActionPerformed

        int port = (serialPort.getSelectedIndex());
        int returnValue = 0;
        returnValue = SecurityObj.ActivateSecurityPort(port);
        textLog.insert("\n Security Activation Return Code. " + returnValue, 0);

    }//GEN-LAST:event_activateSecurityButtonActionPerformed

    private void getBattery_statusActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_getBattery_statusActionPerformed

        int returnValue = 0;
        returnValue = SecurityObj.GetBatteryStatus();

        switch (returnValue) {
            case -1:
                textLog.insert("\n Some Exception Occured.", 0);
                break;

            default:
                textLog.insert("\n " + returnValue + "% Of Battery Remaining.", 0);
                break;
        }// switch(returnValue) end

    }//GEN-LAST:event_getBattery_statusActionPerformed

    private void getUps_statusActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_getUps_statusActionPerformed

        int returnValue = 0;
        returnValue = SecurityObj.GetUPSStatus();

        switch (returnValue) {
            case 0:
                textLog.insert("\n UPS Is On Power Supply.", 0);
                break;
            case 1:
                textLog.insert("\n UPS Is On Battry.", 0);
                break;
            case -1:
                textLog.insert("\n Some Exception Occured.", 0);
                break;
        }// switch(returnValue) end

    }//GEN-LAST:event_getUps_statusActionPerformed

    private void isCashBoxOpenActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_isCashBoxOpenActionPerformed

        int returnValue = 0;
        returnValue = SecurityObj.GetDoorStatus(1);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Cash Box Open.", 0);
                break;

            case 1:
                textLog.insert("\n Cash Box Close.", 0);
                break;
        }// switch(returnValue) end

    }//GEN-LAST:event_isCashBoxOpenActionPerformed

    private void isMainDoorOpenActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_isMainDoorOpenActionPerformed

        int returnValue = 0;
        returnValue = SecurityObj.GetDoorStatus(0);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Main Door Open.", 0);
                break;

            case 1:
                textLog.insert("\n Main Door Closed.", 0);
                break;
        }// switch(returnValue) end

    }//GEN-LAST:event_isMainDoorOpenActionPerformed

    private void disableAlarmActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_disableAlarmActionPerformed

        int returnValue = 0;
        int time = 0;
        if (false == (cashBox_duration.getText()).equals("")) {
            time = Integer.valueOf(cashBox_duration.getText());
            if (time <= 0) {
                JOptionPane.showMessageDialog(null, "Please Input Valid Time Duration.", "Security Management", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(time <= 0) end

        }// if(false == (cashBox_duration.getText()).equals(""))
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Time Duration. ", "Security Management", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end
        //time = Integer.valueOf(cashBox_duration.getText());
        returnValue = SecurityObj.DisableAlarm(0, 0, 1, time);

        switch (returnValue) {
            case 1:
                textLog.insert("\n Alarm Disabled Successfully.", 0);
                break;

            case 0:
                textLog.insert("\n Alarm Disabled Failed.", 0);
                break;
        }// switch(returnValue) end

    }//GEN-LAST:event_disableAlarmActionPerformed

    private void default_Cancle_ButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_default_Cancle_ButtonActionPerformed

        int returnValue = 0;
        //default_Cancle_InputTime;


        int timeOfDefaultCancle = 0;
        //String stringTimeOfReturnNotes = returnNoteTime.getText();

        if (false == default_Cancle_InputTime.getText().equalsIgnoreCase("")) {
            timeOfDefaultCancle = Integer.valueOf(default_Cancle_InputTime.getText());

            if (timeOfDefaultCancle <= 0) {
                JOptionPane.showMessageDialog(null, " Please Input Valid Cancle Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(timeOfReturnNote <= 0) end

            timeOfDefaultCancle = timeOfDefaultCancle * 1000;
        }// if(false == stringTimeOfReturnNotes.equalsIgnoreCase("")) end
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Cancle Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end

        returnValue = CurrencyObj.defaultCancel(timeOfDefaultCancle);

        switch (returnValue) {
            case -1:
                textLog.insert("\n Communication Failure.", 0);
                break;

            case 0:
                textLog.insert("\n Operation Failed.", 0);
                break;

            case 1:
                textLog.insert("\n Operation Successfull.", 0);
                break;

            case 2:
                textLog.insert("\n No Notes in Escrow. Operation cannot be done.", 0);
                break;

        }// switch(returnValue) end

    }//GEN-LAST:event_default_Cancle_ButtonActionPerformed

    private void default_Commit_ButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_default_Commit_ButtonActionPerformed

        int returnValue = 0;
        //default_Commit_InputTime;

        int timeOfDefaultCommit = 0;
        //String stringTimeOfReturnNotes = returnNoteTime.getText();

        if (false == default_Commit_InputTime.getText().equalsIgnoreCase("")) {
            timeOfDefaultCommit = Integer.valueOf(default_Commit_InputTime.getText());

            if (timeOfDefaultCommit <= 0) {
                JOptionPane.showMessageDialog(null, " Please Input Valid Commit Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(timeOfReturnNote <= 0) end

            timeOfDefaultCommit = timeOfDefaultCommit * 1000;
        }// if(false == stringTimeOfReturnNotes.equalsIgnoreCase("")) end
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Commit Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end

        returnValue = CurrencyObj.defaultCommit(timeOfDefaultCommit);

        switch (returnValue) {
            case -1:
                textLog.insert("\n Communication Failure.", 0);
                break;

            case 0:
                textLog.insert("\n Operation Failed.", 0);
                break;

            case 1:
                textLog.insert("\n Operation Successfull.", 0);
                break;

            case 2:
                textLog.insert("\n No Notes in Escrow. Operation cannot be done.", 0);
                break;

        }//switch(returnValue)

    }//GEN-LAST:event_default_Commit_ButtonActionPerformed

    private void noteAccepterStatusButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_noteAccepterStatusButtonActionPerformed

        int rtCode = 0;
        int noRsTen, noRsTwenty, noRsFifty, noRsHundred, noRsFivehundred, noRsThousand;
        byte returnValue, byte0, byte1, byte2, byte3;
        byte0 = 0x00;
        byte1 = 0x00;
        byte2 = 0x00;
        byte3 = 0x00;
        rtCode = CurrencyObj.DeviceStatus(60 * 1000);
        System.out.println(" [ noteAccepterStatusButtonActionPerformed() ] Return Code : " + rtCode);
        byte0 = (byte) (rtCode & 0x000000FF);
        System.out.println(" [ noteAccepterStatusButtonActionPerformed() ] Return Code byte0 : " + byte0);
        byte1 = (byte) ((rtCode & 0x0000FF00) >> 8);
        System.out.println(" [ noteAccepterStatusButtonActionPerformed() ] Return Code byte1 : " + byte1);
        byte2 = (byte) ((rtCode & 0x00FF0000) >> 16);
        System.out.println(" [ noteAccepterStatusButtonActionPerformed() ] Return Code byte2 : " + byte2);
        byte3 = (byte) ((rtCode & 0xFF000000) >> 24);
        System.out.println(" [ noteAccepterStatusButtonActionPerformed() ] Return Code byte3 : " + byte3);

        int b;
        b = byte0 & 0x01;           // bit0
        if (1 == b) // bit0
        {
            textLog.insert("\n Serial Communication is OK. ", 0);

            b = (byte0 & 0x02) >> 1;        // bit1                    
            if (1 == b) // bit1
            {
                textLog.insert("\n Device is Ready. ", 0);

            } else {
                textLog.insert("\n Device is not Ready. ", 0);
            }
        } else {
            textLog.insert("\n Serial Communication is not OK. ", 0);
            //return;
        }

        b = (byte0 & 0x04) >> 2;        // bit2
        if (1 == b) // bit2
        {
            textLog.insert("\n Security Door is Closed. ", 0);

        } else {
            textLog.insert("\n Security Door is Opened. ", 0);
        }

        b = (byte0 & 0x08) >> 3;        // bit3
        if (1 == b) // bit3
        {
            textLog.insert("\n Collection Box is Fulled. ", 0);

        } else {
            textLog.insert("\n Collection Box is Not Full. ", 0);
        }

        b = (byte0 & 0x10) >> 4;        // bit4
        if (1 == b) // bit4
        {
            textLog.insert("\n Insertion Slot is Blocked With Foreign Particle. ", 0);

        }// if(1 == b) end
        else {
            textLog.insert("\n Insertion Slot is Not Blocked With Foreign Particle. ", 0);
        }// else end

        b = (byte0 & 0x20) >> 5;            // bit5
        if (1 == b) // bit5
        {
            textLog.insert("\n Transport Channel is Blocked With Foreign Particle. ", 0);

        }// if(1 == b) end
        else {
            textLog.insert("\n Transport Channel is Not Blocked With Foreign Particle. ", 0);
        }// else end

        noRsTen = byte1 & 0x0F;                     // byte1 No of Rs 10 Note
        if (0 != noRsTen) {
            textLog.insert("\n No of Escrowed INR 10 is " + noRsTen, 0);
        }// if(0 != noRsTen) end

        noRsTwenty = (byte1 & 0xF0) >> 4;           // byte1 No of Rs 20 Note
        if (0 != noRsTwenty) {
            textLog.insert("\n No of Escrowed INR 20 is " + noRsTwenty, 0);
        }// if(0 != noRsTwenty) end

        noRsFifty = byte2 & 0x0F;                   // byte2 No of Rs 50 Note
        if (0 != noRsFifty) {
            textLog.insert("\n No of Escrowed INR 50 is " + noRsFifty, 0);
        }// if(0 != noRsFifty) end

        noRsHundred = (byte2 & 0xF0) >> 4;          // byte2 No of Rs 100 Note
        if (0 != noRsHundred) {
            textLog.insert("\n No of Escrowed INR 100 is " + noRsHundred, 0);
        }// if(0 != noRsHundred) end

        noRsFivehundred = byte3 & 0x0F;             // byte3 No of Rs 500 Note
        if (0 != noRsFivehundred) {
            textLog.insert("\n No of Escrowed INR 500 is " + noRsFivehundred, 0);
        }// if(0 != noRsFivehundred) end

        noRsThousand = (byte3 & 0xF0) >> 4;         // byte4 No of Rs 1000 Note
        if (0 != noRsThousand) {
            textLog.insert("\n No of Escrowed INR 1000 is " + noRsThousand, 0);
        }// if(0 != noRsThousand) end

    }//GEN-LAST:event_noteAccepterStatusButtonActionPerformed

    private void disconnectCurrencyAccepterActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_disconnectCurrencyAccepterActionPerformed


        endingTime = 0;
        totalTime = 0;
        int returnValue;
        startingTime = System.currentTimeMillis();
        returnValue = CurrencyObj.DisConnectDevice(0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ disconnectCurrencyAccepterActionPerformed() ] is " + totalTime);

        switch (returnValue) {

            case -2:
                textLog.insert("\n Any other exception.", 0);
                break;
            case -1:
                textLog.insert("\n Communication failure.", 0);
                break;
            case 0:
                textLog.insert("\n Operation timeout occured.", 0);
                break;
            case 1:
                textLog.insert("\n Accepter disconnected Successfully.", 0);
                DisableAtInitTime();
                break;
            case 2:
                textLog.insert("\n Disconnected successfully but few notes are still in escrow.", 0);
                DisableAtInitTime();
                break;

        }// switch (returnValue) end                

    }//GEN-LAST:event_disconnectCurrencyAccepterActionPerformed

    private void returnNote_movementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_returnNote_movementActionPerformed


        endingTime = 0;
        totalTime = 0;

        int returnValue;
        int timeOfReturnNote = 0;
        String stringTimeOfReturnNotes = returnNoteTime.getText();

        if (false == stringTimeOfReturnNotes.equalsIgnoreCase("")) {
            timeOfReturnNote = Integer.valueOf(stringTimeOfReturnNotes);

            if (timeOfReturnNote <= 0) {
                JOptionPane.showMessageDialog(null, " Please Input Valid Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(timeOfReturnNote <= 0) end

            timeOfReturnNote = timeOfReturnNote * 1000;
        }// if(false == stringTimeOfReturnNotes.equalsIgnoreCase("")) end
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end

        startingTime = System.currentTimeMillis();
        returnValue = CurrencyObj.ReturnAcceptedNotes(timeOfReturnNote);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ returnNote_movementActionPerformed() ] is " + totalTime);

        switch (returnValue) {

            case -3:
                textLog.insert("\n Any other exception.", 0);
                break;
            case -2:
                textLog.insert("\n Return mouth blocked.", 0);
                break;
            case -1:
                textLog.insert("\n Communication failure.", 0);
                break;
            case 0:
                textLog.insert("\n Operation timeout occured.", 0);
                break;
            case 1:
                textLog.insert("\n Returned.", 0);
                break;

        }// switch(returnValue) end                                        
    }//GEN-LAST:event_returnNote_movementActionPerformed

    private void isNoteRemoved_movementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_isNoteRemoved_movementActionPerformed


        endingTime = 0;
        totalTime = 0;

        int returnValue = 0;

        startingTime = System.currentTimeMillis();
        returnValue = CurrencyObj.IsNoteRemoved(0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ isNoteRemoved_movementActionPerformed() ] is " + totalTime);

        System.out.println(String.format("Is not Removed Return Code  -- %d  ", returnValue));

        switch (returnValue) {

            case -2:
                textLog.insert("\n Any other exception.", 0);
                break;
            case -1:
                textLog.insert("\n Communication failure.", 0);
                break;
            case 0:
                textLog.insert("\n Operation timeout occured.", 0);
                break;
            case 1:
                textLog.insert("\n Notes removed.", 0);
                break;
            //case 3: textLog.insert("\n Notes currently in front of Dispencer.");                    
            //    break;
            case 4:
                textLog.insert("\n Notes currently moved to recyling cassete to dispencer.", 0);
                break;
        }// switch(returnValue) end

    }//GEN-LAST:event_isNoteRemoved_movementActionPerformed

    private void stackNote_movementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_stackNote_movementActionPerformed


        endingTime = 0;
        totalTime = 0;

        int returnValue;
        int timeOfStackNote = 0;
        String stringTimeOfStackNotes = stackTime.getText();

        if (false == stringTimeOfStackNotes.equalsIgnoreCase("")) {
            timeOfStackNote = Integer.valueOf(stringTimeOfStackNotes);

            if (timeOfStackNote <= 0) {
                JOptionPane.showMessageDialog(null, " Please Input Valid Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(timeOfStackNote <= 0) end

            timeOfStackNote = timeOfStackNote * 1000;
        }// if(false == stringTimeOfStackNotes.equalsIgnoreCase("")) end
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end


        startingTime = System.currentTimeMillis();
        returnValue = CurrencyObj.StackAcceptedNotes(timeOfStackNote);

        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ stackNote_movementActionPerformed() ] is " + totalTime);

        switch (returnValue) {

            case -4:
                textLog.insert("\n Any other exception.", 0);
                break;
            case -3:
                textLog.insert("\n Transport Channel Blocked.", 0);
                break;
            case -2:
                textLog.insert("\n Stacker/Cash Box full.", 0);
                break;
            case -1:
                textLog.insert("\n Communication failure.", 0);
                break;
            case 0:
                textLog.insert("\n Operation timeout occured.", 0);
                break;
            case 1:
                textLog.insert("\n Stacked.", 0);
                break;

        }// switch(returnValue) end

    }//GEN-LAST:event_stackNote_movementActionPerformed

    private void accept_multinotesActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_accept_multinotesActionPerformed

        startingTime = System.currentTimeMillis();
        endingTime = 0;
        totalTime = 0;

        boolean returnValue = false;
        String rs;
        rs = multiAmount_rs.getText();
        sec = timeInSec.getText();

        if (false == rs.equals("")) {
            Amount = Integer.valueOf(rs);
            if (Amount <= 0) {
                JOptionPane.showMessageDialog(null, "Please Input Valid Amount.", "Currency Management", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(Amount <= 0) end

        }// if(false == rs.equals("")) end
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Amount. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end

        if (false == sec.equalsIgnoreCase("")) {
            Time = Integer.valueOf(sec);

            if (Time <= 0) {
                JOptionPane.showMessageDialog(null, " Please Input Valid Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(Time <= 0) end

            Time = Time * 1000;
        }// if(false == sec.equalsIgnoreCase("")) end
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end                        

        startingTime = System.currentTimeMillis();

        Thread multiNoteThread = new Thread(this) {

            public void run() {
                multiNoteTimeRem.setText(sec);
                int timeout = 0;
                threadflag = true;
                while (threadflag) {

                    System.out.println(" Threadflag for loop");
                    try {
                        System.out.println(" Thread sleep");
                        Thread.sleep(1000);

                    }// try end           
                    catch (InterruptedException ie) {
                        System.out.println(" [ note_validateActionPerformed() ] exception in thread sleep.");

                    }// catch(InterruptedException ie ) end

                    multiNoteTimeRem.setText(String.valueOf(Integer.valueOf(timeInSec.getText()) - timeout));

                    timeout++;

                    if (Integer.valueOf(timeInSec.getText()) < timeout) {
                        threadflag = false;
                        multiNoteTimeRem.setText("");
                    }// if(Integer.valueOf(timeInSec.getText())<timeout) end

                }// while(threadflag) end

                multiNoteTimeRem.setText("");

            }// public void run() end
        };// Thread multiNoteThread = new Thread(this) end

        multiNoteThread.start();

        returnValue = CurrencyObj.AcceptNotes(Amount, Time);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ accept_multinotesActionPerformed() ] is " + totalTime);

        if (returnValue == false) {
            textLog.insert("\n Accepter is not ready to accept currency.", 0);
        }// (returnValue==false) end
        else {
            textLog.insert("\n Accepter is in acceptance state.", 0);
            flag = true;
            t1 = new Thread(this);
            t1.start();
        }// else end

    }//GEN-LAST:event_accept_multinotesActionPerformed

    private void enable_denomsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_enable_denomsActionPerformed

        endingTime = 0;
        totalTime = 0;

        int returnValue=0;
        startingTime = System.currentTimeMillis();
        returnValue = CurrencyObj.EnableTheseDenominations(127, 0);

        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ enable_denomsActionPerformed() ] is " + totalTime);
        
        System.out.println("EnableTheseDenominations Return Value: "+returnValue);
        
        switch (returnValue) {

            case -2:
                textLog.insert("\n Any other exception.", 0);
                break;
            case -1:
                textLog.insert("\n Communication failure.", 0);
                break;
            case 0:
                textLog.insert("\n Operation timeout occured.", 0);
                break;
            case 1:
                textLog.insert("\n Requested denominations enabled.", 0);
                enable_denoms.setEnabled(false);
                break;

        }// switch (returnValue) end

    }//GEN-LAST:event_enable_denomsActionPerformed

    private void note_returnActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_note_returnActionPerformed


        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = CurrencyObj.ReturnCurrentNote(60 * 1000);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ note_returnActionPerformed() ] is " + totalTime);

        switch (returnValue) {

            case -3:
                textLog.insert("\n Any other exception.", 0);
                break;

            case -2:
                textLog.insert("\n Return mouth blocked.", 0);
                break;

            case -1:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 0:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 1:
                textLog.insert("\n Returned.", 0);
                break;

        }// switch(returnValue) end                

    }//GEN-LAST:event_note_returnActionPerformed

    private void note_escrowActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_note_escrowActionPerformed

        endingTime = 0;
        totalTime = 0;
        int singleDenom = 0;
        int returnValue = 0;

        if (false == singleNoteDenom.getText().equals("")) {

            singleDenom = Integer.valueOf(singleNoteDenom.getText());

            if (singleDenom <= 0) {
                JOptionPane.showMessageDialog(null, " Please Input Valid Amount ", "Currency Management", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(singleDenom <= 0) end

        }// if(false == singleNoteDenom.getText().equals("")) end
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Amount ", "Currency Management", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end

        startingTime = System.currentTimeMillis();
        returnValue = CurrencyObj.AcceptCurrentNote(singleDenom, 60 * 1000);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ note_escrowActionPerformed() ] is " + totalTime);

        switch (returnValue) {

            case -3:
                textLog.insert("\n Any other exception.", 0);
                break;

            case -2:
                textLog.insert("\n Escrow full.", 0);
                break;

            case -1:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 0:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 1:
                textLog.insert("\n Note of correct denomination accepted.", 0);
                break;

        }// switch(returnValue) end                

    }//GEN-LAST:event_note_escrowActionPerformed

    private void note_validateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_note_validateActionPerformed

        endingTime = 0;
        totalTime = 0;
        denomTime = 0;

        if (false == singleNoteDenom.getText().equals("")) {

            singleDenom = Integer.valueOf(singleNoteDenom.getText());

            if (singleDenom <= 0) {
                JOptionPane.showMessageDialog(null, " Please Input Valid Amount ", "Currency Management", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(singleDenom <= 0) end

        }// (false == singleNoteDenom.getText().equals("")) end
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Amount ", "Currency Management", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end

        if (false == (singleDenomTime.getText()).equalsIgnoreCase("")) {
            denomTime = Integer.valueOf(singleDenomTime.getText());

            if (denomTime <= 0) {
                JOptionPane.showMessageDialog(null, " Please Input Valid Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(Time <= 0) end

            denomTime = denomTime * 1000;
        }// if(false == sec.equalsIgnoreCase("")) end
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Time. ", "Currency Management", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end                        

        timerThread = new Thread(this) {

            public void run() {
                validateTime.setText(String.valueOf(denomTime / 1000));
                int timeout = 0;
                threadflag = true;
                while (threadflag) {

                    System.out.println(" Threadflag for loop");
                    try {
                        System.out.println(" Thread sleep");
                        Thread.sleep(1000);

                    }// try end
                    catch (InterruptedException ie) {
                        System.out.println(" [ note_validateActionPerformed() ] exception in thread sleep.");

                    }// catch(InterruptedException ie ) end

                    validateTime.setText(String.valueOf(denomTime / 1000 - timeout));

                    timeout++;

                    if (denomTime / 1000 < timeout) {

                        threadflag = false;
                        validateTime.setText("");

                    }// if(denomTime/1000<timeout) end

                }// while

                validateTime.setText("");

            }// timerthread public void run() 
        };
        timerThread.start();
        //timerThreadstart();

        Thread SingleD = new Thread() {

            @Override
            public void run() {
                System.out.println(" Worker Thread Background ");
                rtValue = CurrencyObj.GetValidNote(singleDenom, denomTime);
                flagset();
                textLog.insert("\n Return Value of Single Note Denom " + rtValue + "\n", 0);
                //threadflag = false;
                switch (rtValue) {

                    case -4:
                        textLog.insert("\n Any other exception.\n", 0);
                        break;

                    case -3:
                        textLog.insert("\n Note rejected due to insertion of an invalid (mutilated/soiled/non-gandhi series note/counterfeit) note.\n", 0);
                        break;

                    case -2:
                        textLog.insert("\n Note rejected due to insertion of a disallowed denomination.\n", 0);
                        break;

                    case -1:
                        textLog.insert("\n Communication failure.\n", 0);
                        break;

                    case 0:
                        textLog.insert("\n Operation timeout occurred.\n", 0);
                        break;

                    case 1:
                        textLog.insert("\n Note of correct denomination validated.\n", 0);
                        break;

                }// switch(returnValue)end

            }
        };
        SingleD.start();

        //startingTime=System.currentTimeMillis(); 
        //returnValue=CurrencyObj.GetValidNote(singleDenom, 60*1000);            

        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ note_validateActionPerformed() ] is " + totalTime);

    }//GEN-LAST:event_note_validateActionPerformed

    private void currencyAccepter_returnActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_currencyAccepter_returnActionPerformed

        escrowClearenceMode = 2;
    }//GEN-LAST:event_currencyAccepter_returnActionPerformed

    private void currencyAccepter_stackActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_currencyAccepter_stackActionPerformed

        escrowClearenceMode = 1;

    }//GEN-LAST:event_currencyAccepter_stackActionPerformed

    private void currencyAccepter_keepActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_currencyAccepter_keepActionPerformed

        escrowClearenceMode = 0;

    }//GEN-LAST:event_currencyAccepter_keepActionPerformed

    private void currencyAccepter_connectActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_currencyAccepter_connectActionPerformed


        endingTime = 0;
        totalTime = 0;
        int port = (serialPort.getSelectedIndex());
        System.out.println(port);
        int returnValue;
        returnValue = -1;
        startingTime = System.currentTimeMillis();
        returnValue = CurrencyObj.ConnectDevice(port, escrowClearenceMode, 0);      //Connecting Device

        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ currencyAccepter_connectActionPerformed() ] is " + totalTime);



        switch (returnValue) {
            case -7:
                textLog.insert("\n Any other exception.", 0);
                break;
            case -6:
                textLog.insert("\n Escrow clearence failed due to unknown reason.", 0);
                break;
            case -5:
                textLog.insert("\n Escrow clearence failed due to any blocking.", 0);
                break;
            case -4:
                textLog.insert("\n Escrow clearence failed due to collection bin full.", 0);
                break;
            case -3:
                textLog.insert("\n Security door is open.", 0);
                break;
            case -2:
                textLog.insert("\n Port doesn't exist.", 0);
                break;
            case -1:
                textLog.insert("\n Communication failure.", 0);
                break;
            case 0:
                textLog.insert("\n Operation timeout occured.", 0);
                break;
            case 1:
                textLog.insert("\n Device connected Succesfully.", 0);
                EnableAtInitTime();
                break;
            case 2:
                textLog.insert("\n Device connected succesfully with few notes escrowed.", 0);
                EnableAtInitTime();
                break;

        }// switch() end

    }//GEN-LAST:event_currencyAccepter_connectActionPerformed

    private void contactCard_wormresetActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_contactCard_wormresetActionPerformed

        endingTime = 0;
        totalTime = 0;
        byte[] returnValue = null;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.ResetSAM(1, 1, 0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ contactCard_wormresetActionPerformed() ] is " + totalTime);

        if (null == returnValue) {
            return;
        }// if( null == returnValue ) end
        else {
            int counter = 0;
            for (int b : returnValue) {
                counter++;
                switch (counter) {
                    case 1:
                        switch (b) {
                            case 0:
                                textLog.insert("\n No card (SAM) found. ", 0);
                                return;

                            case 1:
                                textLog.insert("\n Operation successful. ", 0);
                                break;

                            case 2:
                                textLog.insert("\n Operation failed. ", 0);
                                break;

                            case 3:
                                textLog.insert("\n Communication failure. ", 0);
                                return;

                            case 4:
                                textLog.insert("\n Operation timeout occurred. ", 0);
                                return;

                            case 5:
                                textLog.insert("\n Other error. ", 0);
                                return;

                        }// switch(b) end
                        break;

                    default:
                        textLog.insert(String.valueOf(b), 0);
                        break;

                }// switch(counter) end

            }// for( int b : returnValue ) end

        }// else end

    }//GEN-LAST:event_contactCard_wormresetActionPerformed

    private void contactCard_poweroffActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_contactCard_poweroffActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.SAMSlotPowerOnOff(1, 0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ contactCard_poweroffActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Operation Failed.", 0);
                break;

            case 1:
                textLog.insert("\n Operation Succeedd.", 0);
                break;
        }// switch(returnValue) end                

    }//GEN-LAST:event_contactCard_poweroffActionPerformed

    private void send_tActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_send_tActionPerformed


  }//GEN-LAST:event_send_tActionPerformed

    private void contactCard_coldresetActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_contactCard_coldresetActionPerformed

        endingTime = 0;
        totalTime = 0;
        byte[] returnValue = null;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.ResetSAM(1, 0, 0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ contactCard_coldresetActionPerformed() ] is " + totalTime);

        if (null == returnValue) {
            return;
        }// if( null == returnValue ) end
        else {
            int counter = 0;
            for (int b : returnValue) {
                counter++;
                switch (counter) {
                    case 1:
                        switch (b) {
                            case 0:
                                textLog.insert("\n No card (SAM) found. ", 0);
                                return;

                            case 1:
                                textLog.insert("\n Operation successful. ", 0);
                                break;

                            case 2:
                                textLog.insert("\n Operation failed. ", 0);
                                break;

                            case 3:
                                textLog.insert("\n Communication failure. ", 0);
                                return;

                            case 4:
                                textLog.insert("\n Operation timeout occurred. ", 0);
                                return;

                            case 5:
                                textLog.insert("\n Other error. ", 0);
                                return;

                        }// switch(b) end
                        break;

                    default:
                        textLog.insert(String.valueOf(b), 0);
                        break;

                }// switch(counter) end

            }// for( int b : returnValue ) end

        }// else end        

    }//GEN-LAST:event_contactCard_coldresetActionPerformed

    private void contactCard_deactivateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_contactCard_deactivateActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.DeactivateCard(1, 1, 0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ contactCard_deactivateActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n No card found.", 0);
                break;

            case 1:
                textLog.insert("\n Card found and deactivated.", 0);
                break;

            case 2:
                textLog.insert("\n Card found but deactivation failed.", 0);
                break;

            case 3:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 4:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 5:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end

    }//GEN-LAST:event_contactCard_deactivateActionPerformed

    private void contactCard_poweronActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_contactCard_poweronActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.SAMSlotPowerOnOff(1, 1);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ contactCard_poweronActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Operation Failed.", 0);
                break;

            case 1:
                textLog.insert("\n Operation Succeedd.", 0);
                break;
        }// switch(returnValue) end

    }//GEN-LAST:event_contactCard_poweronActionPerformed

    private void contactCard_activateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_contactCard_activateActionPerformed

        endingTime = 0;
        totalTime = 0;
        byte[] returnValue = null;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.ActivateCard(1, 1, 0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ contactCard_activateActionPerformed() ] is " + totalTime);

        if (null == returnValue) {
            return;
        }// if( null == returnValue ) end
        else {
            int counter = 0;
            for (int b : returnValue) {
                counter++;
                switch (counter) {
                    case 1:
                        switch (b) {
                            case 0:
                                textLog.insert("\n No card found. ", 0);
                                return;

                            case 1:
                                textLog.insert("\n Card found and activated. ", 0);
                                break;

                            case 2:
                                textLog.insert("\n Card found but activation failed. ", 0);
                                break;

                            case 3:
                                textLog.insert("\n Card found but it is unsupported. ", 0);
                                break;

                            case 4:
                                textLog.insert("\n Communication failure. ", 0);
                                return;

                            case 5:
                                textLog.insert("\n Operation timeout occurred. ", 0);
                                return;

                            case 6:
                                textLog.insert("\n Other error. ", 0);
                                return;

                        }// switch(b) end
                        break;

                    case 2:
                        switch (b) {
                            case 1:
                                textLog.insert("\n Card type : MIFARE DESFire ", 0);
                                break;

                            case 2:
                                textLog.insert("\n Card type : MIFARE DESFire EV1 ", 0);
                                break;

                            case 3:
                                textLog.insert("\n Card type : MIFARE Ultralight ", 0);
                                break;

                        }// switch(b) end
                        break;

                    case 3:
                        textLog.insert("\n Size of UID " + b, 0);
                        break;

                    case 4:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 5:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 6:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 7:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 8:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 9:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 10:
                        textLog.insert("\n UID " + b, 0);
                        break;

                }// switch(counter) end

            }// for( int b : returnValue ) end

        }// else end                

    }//GEN-LAST:event_contactCard_activateActionPerformed

    private void sendT_clActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_sendT_clActionPerformed

        byte[] returnValue = null;
        byte cmdArray[] =null;
        //default_Cancle_InputTime;

        int timeForApdu = 0;
        //String stringTimeOfReturnNotes = returnNoteTime.getText();
        //String cmd = null;
        //cmd = apduCmd.getText();
        
        int lenghthofarray = 0;
        String apdustr = null;
        
        //++apdustr = apduCmd.getText();
        //++byte cmdArray[] = apduCmd.getText().getBytes();
        //++JOptionPane.showMessageDialog(this, apduCmd.getText().replaceAll(" ","") );
        
        /*
        String withoutspaces = "";
        for (int i = 0; i < apdustr.length(); i++) {

		if (apdustr.charAt(i) != ' '){
                    //++JOptionPane.showMessageDialog(this, "Found WhiteSpace" );
		    withoutspaces += apdustr.charAt(i);
                }
        }
        
        
        try{ 
            cmdArray = withoutspaces.getBytes("UTF-8");
        }catch(java.io.UnsupportedEncodingException ex){
            JOptionPane.showMessageDialog(this, "Failed data read error" );
            return;
        } 

        JOptionPane.showMessageDialog(this, "ADPU_ARRAY: "+withoutspaces );
        JOptionPane.showMessageDialog(this, "Byte Array Length:"+cmdArray.length );
        */

        /*
               Sample:
	       APDU Command:
	       STX+ADDR+LENH+LENL+CMT+CM+PM>>F2 00 00 16 43 60 34 
	       APDU COMMAND>>00 A4 04 00 0E 31 50 41 59 2E 53 59 53 2E 44 44 46 30 31 
	       ETX+BCC>>03 39 

	       APDU Command Reply:
	       STX+ADDR+LENH+LENL+PMT+CM+PM+ST0+ST1+ST2>>F2 00 00 08 50 60 34 32 30 30 
	       DATA>>6A 87 
	       ETX+BCC>>03 22 


        */
        
        //0x00 A4 04 00 0E 31 50 41 59 2E 53 59 53 2E 44 44 46 30 31 
        cmdArray = new byte[19];

        cmdArray[0] =0x00;
        cmdArray[1] =(byte)0xA4;
        cmdArray[2] =0x04;
        cmdArray[3] =0x00;
        cmdArray[4] =0x0E;
        cmdArray[5] =0x31;
        cmdArray[6] =0x50;
        cmdArray[7] =0x41;
        cmdArray[8] =0x59;
        cmdArray[9] =0x2E;
        cmdArray[10] =0x53;
        cmdArray[11] =0x59;
        cmdArray[12] =0x53;
        cmdArray[13] =0x2E;
        cmdArray[14] =0x44;
        cmdArray[15] =0x44;
        cmdArray[16] =0x46;
        cmdArray[17] =0x30;
        cmdArray[18] =0x31;
       
        StringBuilder cmdsb = new StringBuilder();
	    for (byte b : cmdArray) {
		cmdsb.append(String.format("%02X ", b));
	    }
        
        JOptionPane.showMessageDialog(this, "ADPU_ARRAY: "+cmdsb.toString() );
        JOptionPane.showMessageDialog(this, "Byte Array Length:"+cmdArray.length );
        
        if (false == apduTime.getText().equalsIgnoreCase("")) {
            timeForApdu = Integer.valueOf(apduTime.getText());

            if (timeForApdu <= 0) {
                JOptionPane.showMessageDialog(null, " Please Input Valid Cancle Time. ", "Card Read Write", JOptionPane.WARNING_MESSAGE);
                return;
            }// if(timeOfReturnNote <= 0) end

            timeForApdu = timeForApdu * 1000;
        }// if(false == stringTimeOfReturnNotes.equalsIgnoreCase("")) end
        else {
            JOptionPane.showMessageDialog(null, " Please Input Valid Cancle Time. ", "Card Read Write", JOptionPane.WARNING_MESSAGE);
            return;
        }// else end


        returnValue = SmartCardObj.XChangeAPDU(0, cmdArray, 0, timeForApdu);
        lenghthofarray = returnValue.length;
        if (null == returnValue) {
            return;
        }// if( null == returnValue ) end
        else {
            /*
            int counter = 0;
            for (int b : returnValue) {

                counter++;
                switch (counter) {
                    case 1:
                        switch (b) {
                            case 0:
                                textLog.insert("\n Error in APDU Command ", 0);
                                return;

                            case 1:
                                textLog.insert("\n APDU Operation Successfully Done. ", 0);
                                break;

                            case 2:
                                textLog.insert("\n Communication Failure. ", 0);
                                break;

                            case 3:
                                textLog.insert("\n Operation timeout occurred. ", 0);
                                break;

                            case 4:
                                textLog.insert("\n Other error. ", 0);
                                return;

                            default:
                                  textLog.insert("\n Unknown Error ", 0);
                                  return;

                        }// switch(b) end
                        break;

                    /*
                     * case 2: switch(b) { case 1: textLog.insert("\n Card type
                     * : MIFARE DESFire ", 0); break;
                     *
                     * case 2: textLog.insert("\n Card type : MIFARE DESFire EV1
                     * ", 0); break;
                     *
                     * case 3: textLog.insert("\n Card type : MIFARE Ultralight
                     * ", 0); break;
                     *
                     * }// switch(b) end break;
                     *
                     * case 3: textLog.insert("\n Size of UID "+b, 0); break;
                     *
                     * case 4: textLog.insert("\n UID "+b, 0); break;
                     *
                     * case 5: textLog.insert("\n UID "+b, 0); break;
                     *
                     * case 6: textLog.insert("\n UID "+b, 0); break;
                     *
                     * case 7: textLog.insert("\n UID "+b, 0); break;
                     *
                     * case 8: textLog.insert("\n UID "+b, 0); break;
                     *
                     * case 9: textLog.insert("\n UID "+b, 0); break;
                     *
                     * case 10: textLog.insert("\n UID "+b, 0); break;
                     *
                     *

                }// switch(counter) end

            }// for( int b : returnValue ) end
            */
            //+=System.out.println(returnValue);
            String rslt = new String(returnValue);
            //textLog.insert("\n" + rslt, 0);
            StringBuilder sb = new StringBuilder();
	    for (byte b : returnValue) {
		sb.append(String.format("%02X ", b));
	    }
            JOptionPane.showMessageDialog(this, "Return APDU Reply:"+sb.toString() );

        }// else

    }//GEN-LAST:event_sendT_clActionPerformed

    private void read_desActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_read_desActionPerformed


  }//GEN-LAST:event_read_desActionPerformed

    private void contactlessCard_deactivateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_contactlessCard_deactivateActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.DeactivateCard(0, 0, 0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ contactlessCard_deactivateActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n No card found.", 0);
                break;

            case 1:
                textLog.insert("\n Card found and deactivated.", 0);
                break;

            case 2:
                textLog.insert("\n Card found but deactivation failed.", 0);
                break;

            case 3:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 4:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 5:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end                

    }//GEN-LAST:event_contactlessCard_deactivateActionPerformed

    private void contactlessCard_activateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_contactlessCard_activateActionPerformed

        endingTime = 0;
        totalTime = 0;
        byte[] returnValue = null;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.ActivateCard(0, 0, 0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ contactlessCard_activateActionPerformed() ] is " + totalTime);

        if (null == returnValue) {
            return;
        }// if( null == returnValue ) end
        else {
            int counter = 0;
            for (int b : returnValue) {

                counter++;
                switch (counter) {
                    case 1:
                        switch (b) {
                            case 0:
                                textLog.insert("\n No card found. ", 0);
                                return;

                            case 1:
                                textLog.insert("\n Card found and activated. ", 0);
                                break;

                            case 2:
                                textLog.insert("\n Card found but activation failed. ", 0);
                                break;

                            case 3:
                                textLog.insert("\n Card found but it is unsupported. ", 0);
                                break;

                            case 4:
                                textLog.insert("\n Communication failure. ", 0);
                                return;

                            case 5:
                                textLog.insert("\n Operation timeout occurred. ", 0);
                                return;

                            case 6:
                                textLog.insert("\n Other error. ", 0);
                                return;

                        }// switch(b) end
                        break;

                    case 2:
                        switch (b) {
                            case 1:
                                textLog.insert("\n Card type : MIFARE DESFire ", 0);
                                break;

                            case 2:
                                textLog.insert("\n Card type : MIFARE DESFire EV1 ", 0);
                                break;

                            case 3:
                                textLog.insert("\n Card type : MIFARE Ultralight ", 0);
                                break;

                        }// switch(b) end
                        break;

                    case 3:
                        textLog.insert("\n Size of UID " + b, 0);
                        break;

                    case 4:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 5:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 6:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 7:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 8:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 9:
                        textLog.insert("\n UID " + b, 0);
                        break;

                    case 10:
                        textLog.insert("\n UID " + b, 0);
                        break;

                }// switch(counter) end

            }// for( int b : returnValue ) end

        }// else end

    }//GEN-LAST:event_contactlessCard_activateActionPerformed

    private void fullEjectCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_fullEjectCardActionPerformed

        cardReturnMode = 1;
        halfEjectCard.setSelected(false);

    }//GEN-LAST:event_fullEjectCardActionPerformed

    private void halfEjectCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_halfEjectCardActionPerformed

        cardReturnMode = 0;
        fullEjectCard.setSelected(false);

    }//GEN-LAST:event_halfEjectCardActionPerformed

    private void cardReaderStatusButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cardReaderStatusButtonActionPerformed

        byte[] returnValue = null;
        byte byte0, byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8, byte9, byte10;
        int rtValue = 0;
        int bitValue = 0;
        returnValue = SmartCardObj.GetDeviceStatus(0, 10 * 1000);

        if (null == returnValue) {
            textLog.insert(" SmartCardApi returns NULL ", 0);
            return;
        }// if(null == returnValue) end
        else {
            if (0 == returnValue.length) {

                textLog.insert(" SmartCardApi array length 0 ", 0);
                return;

            }// if(0 == returnValue.length) end
            else {
                try {
                    byte0 = returnValue[0];
                    rtValue = byte0 * 0xFF;
                    switch (rtValue) {
                        case 0:
                            textLog.insert("\n Operation Successfull. ", 0);
                            break;

                        case 1:
                            textLog.insert("\n Communication Failure.  ", 0);
                            break;

                        case 2:
                            textLog.insert("\n Operation Timeout Occurred. ", 0);
                            break;

                        case 3:
                            textLog.insert("\n Other Error. ", 0);
                            break;
                    }// switch(rtValue) end

                    byte1 = returnValue[1];
                    rtValue = byte1 * 0xFF;
                    switch (rtValue) {
                        case 0:
                            textLog.insert("\n RFID Reader Ready. ", 0);
                            break;

                        case 1:
                            textLog.insert("\n RFID Reader Not Ready. ", 0);
                            break;

                    }// switch(rtValue) end

                    byte2 = returnValue[2];
                    rtValue = byte2 * 0xFF;
                    switch (rtValue) {
                        case 0:
                            textLog.insert("\n SAM Reader Ready. ", 0);
                            break;

                        case 1:
                            textLog.insert("\n SAM Reader Not Ready. ", 0);
                            break;

                    }// switch(rtValue) end

                    byte3 = returnValue[3];
                    rtValue = byte3 * 0xFF;
                    switch (rtValue) {
                        case 0:
                            textLog.insert("\n Stacker Empty. ", 0);
                            break;

                        case 1:
                            textLog.insert("\n Stacker Nearly Empty. ", 0);
                            break;

                        case 2:
                            textLog.insert("\n Stacker Nearly Full. ", 0);
                            break;

                        case 3:
                            textLog.insert("\n Stacker Full. ", 0);
                            break;

                    }// switch(rtValue) end

                    byte4 = returnValue[4];
                    rtValue = byte4 * 0xFF;
                    textLog.insert("\n Approx Card Count in Stacker " + rtValue, 0);

                    byte5 = returnValue[5];
                    rtValue = byte5 * 0xFF;
                    switch (rtValue) {
                        case 0:
                            textLog.insert("\n Rejection Bin Empty. ", 0);
                            break;

                        case 1:
                            textLog.insert("\n Rejection Bin Nearly Empty. ", 0);
                            break;

                        case 2:
                            textLog.insert("\n Rejection Bin Nearly Full. ", 0);
                            break;

                        case 3:
                            textLog.insert("\n Rejection Bin Full. ", 0);
                            break;

                    }// switch(rtValue) end

                    byte6 = returnValue[6];
                    rtValue = byte6 * 0xFF;
                    textLog.insert("\n Approx Card Count in Rejection Bin. " + rtValue, 0);

                    byte7 = returnValue[7];
                    //rtValue = byte7 * 0xFF;
                    switch (byte7) {
                        case 0:
                            textLog.insert("\n Channel Clear. ", 0);
                            break;

                        case 1:
                            textLog.insert("\n Channel Blocked. ", 0);
                            break;

                        case 2:
                            textLog.insert("\n Forcefully Card Inserted. ", 0);
                            break;

                    }// switch(rtValue) end

                    byte8 = returnValue[8];
                    rtValue = byte8 * 0xFF;
                    textLog.insert("\n Channel Sensor Stat " + rtValue, 0);

                    bitValue = byte8 * 0x01;
                    if (0 == bitValue) {
                        textLog.insert("\n Sensor 1 Clear. ", 0);
                    } else {
                        textLog.insert("\n Sensor 1 Blocked. ", 0);
                    }

                    bitValue = byte8 * 0x02 >> 1;
                    if (0 == bitValue) {
                        textLog.insert("\n Sensor 2 Clear. ", 0);
                    } else {
                        textLog.insert("\n Sensor 2 Blocked. ", 0);
                    }

                    bitValue = byte8 * 0x04 >> 2;
                    if (0 == bitValue) {
                        textLog.insert("\n Sensor 3 Clear. ", 0);
                    } else {
                        textLog.insert("\n Sensor 3 Blocked. ", 0);
                    }

                    bitValue = byte8 * 0x08 >> 3;
                    if (0 == bitValue) {
                        textLog.insert("\n Sensor 4 Clear. ", 0);
                    } else {
                        textLog.insert("\n Sensor 4 Blocked. ", 0);
                    }

                    bitValue = byte8 * 0x10 >> 4;
                    if (0 == bitValue) {
                        textLog.insert("\n Sensor 5 Clear. ", 0);
                    } else {
                        textLog.insert("\n Sensor 5 Blocked. ", 0);
                    }

                    bitValue = byte8 * 0x20 >> 5;
                    if (0 == bitValue) {
                        textLog.insert("\n Sensor 6 Clear. ", 0);
                    } else {
                        textLog.insert("\n Sensor 6 Blocked. ", 0);
                    }

                    bitValue = byte8 * 0x40 >> 6;
                    if (0 == bitValue) {
                        textLog.insert("\n Sensor 7 Clear. ", 0);
                    } else {
                        textLog.insert("\n Sensor 7 Blocked. ", 0);
                    }

                    bitValue = byte8 * 0x80 >> 7;
                    if (0 == bitValue) {
                        textLog.insert("\n Sensor 8 Clear. ", 0);
                    } else {
                        textLog.insert("\n Sensor 8 Blocked. ", 0);
                    }


                    byte9 = returnValue[9];
                    rtValue = byte9 * 0xFF;
                    switch (rtValue) {
                        case 0:
                            textLog.insert("\n Collection Bin Empty. ", 0);
                            break;

                        case 1:
                            textLog.insert("\n Collection Bin Nearly Empty. ", 0);
                            break;

                        case 2:
                            textLog.insert("\n Collection Bin Nearly Full. ", 0);
                            break;

                        case 3:
                            textLog.insert("\n Collection Bin Full. ", 0);
                            break;

                    }// switch(rtValue) end

                    byte10 = returnValue[10];
                    rtValue = byte10 * 0xFF;
                    textLog.insert("\n Card Count in Collection Bin. " + rtValue, 0);

                }// try end
                catch (ArrayIndexOutOfBoundsException aex) {
                    System.out.println(" [ ArrayIndexOutOfBoundsException () ] Device Status report");

                }// catch(ArrayIndexOutOfBoundsException aex) end

            }// else end

        }// else end
    }//GEN-LAST:event_cardReaderStatusButtonActionPerformed

    private void initReturnCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_initReturnCardActionPerformed
        cardinitmode = 2;
        initKeepCard.setSelected(false);
        initCollectCard.setSelected(false);
    }//GEN-LAST:event_initReturnCardActionPerformed

    private void initCollectCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_initCollectCardActionPerformed
        cardinitmode = 1;
        initKeepCard.setSelected(false);
        initReturnCard.setSelected(false);
    }//GEN-LAST:event_initCollectCardActionPerformed

    private void initKeepCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_initKeepCardActionPerformed
        cardinitmode = 0;
        initCollectCard.setSelected(false);
        initReturnCard.setSelected(false);
    }//GEN-LAST:event_initKeepCardActionPerformed

    private void cardReader_InitButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cardReader_InitButtonActionPerformed

        SmartCardObj.InitMutek(cardinitmode);
    }//GEN-LAST:event_cardReader_InitButtonActionPerformed

    private void disconnectCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_disconnectCardActionPerformed

        endingTime = 0;
        totalTime = 0;

        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.DisConnectDevice(0);
        System.out.println(" [ disconnectCardActionPerformed() ] return value: " + returnValue);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ disconnectCardActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Disconnected successfully.", 0);
                break;

            case 1:
                textLog.insert("\n Disconnected successfully but a card is in the channel.", 0);
                break;

            case 2:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 3:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 4:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end

    }//GEN-LAST:event_disconnectCardActionPerformed

    private void cardRemovedActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cardRemovedActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.IsCardRemoved(0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for connectCardActionPerformed is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Not removed.", 0);
                break;

            case 1:
                textLog.insert("\n Removed.", 0);
                break;

            case 2:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 3:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 4:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end                

    }//GEN-LAST:event_cardRemovedActionPerformed

    private void cardInChannelActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cardInChannelActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.IsCardInChannel(0);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for connectCardActionPerformed is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n No card in the channel.", 0);
                break;

            case 1:
                textLog.insert("\n Card found in the channel.", 0);
                break;

            case 2:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 3:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 4:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end


    }//GEN-LAST:event_cardInChannelActionPerformed

    private void collectCard_movementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_collectCard_movementActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.CollectCard(0);
        System.out.println(" [ collectCard_movementActionPerformed() ] return value: " + returnValue);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for connectCardActionPerformed is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Operation successfully.", 0);
                break;

            case 1:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 2:
                textLog.insert("\n Rejection bin full.", 0);
                break;

            case 3:
                textLog.insert("\n No card in the channel.", 0);
                break;

            case 4:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 5:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end                

    }//GEN-LAST:event_collectCard_movementActionPerformed

    private void returnCard_movementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_returnCard_movementActionPerformed

        endingTime = 0;
        totalTime = 0;

        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.ReturnCard(0, cardReturnMode);
        System.out.println(" [ returnCard_movementActionPerformed() ] cardReturnMode value: " + cardReturnMode);
        System.out.println(" [ returnCard_movementActionPerformed() ] return value: " + returnValue);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ returnCard_movementActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Operation successfully.", 0);
                break;

            case 1:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 2:
                textLog.insert("\n Return mouth blocked.", 0);
                break;

            case 3:
                textLog.insert("\n No card in the channel.", 0);
                break;

            case 4:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 5:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end                

    }//GEN-LAST:event_returnCard_movementActionPerformed

    private void rejectCard_movementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_rejectCard_movementActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.RejectCard(0);
        System.out.println(" [ rejectCard_movementActionPerformed() ] return value: " + returnValue);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ dispanseCard_movementActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Operation successfully.", 0);
                break;

            case 1:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 2:
                textLog.insert("\n Rejection bin full.", 0);
                break;

            case 3:
                textLog.insert("\n No card in the channel.", 0);
                break;

            case 4:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 5:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end                

    }//GEN-LAST:event_rejectCard_movementActionPerformed

    private void dispanseCard_movementActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_dispanseCard_movementActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.DispenseCard(0);
        System.out.println(" [ dispanseCard_movementActionPerformed() ] return value: " + returnValue);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ dispanseCard_movementActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Operation successfully.", 0);
                break;

            case 1:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 2:
                textLog.insert("\n Channel blocked.", 0);
                break;

            case 3:
                textLog.insert("\n Insertion/return mouth blocked.", 0);
                break;

            case 4:
                textLog.insert("\n Stacker empty", 0);
                break;

            case 5:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 6:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end        

    }//GEN-LAST:event_dispanseCard_movementActionPerformed

    private void acceptCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_acceptCardActionPerformed

        endingTime = 0;
        totalTime = 0;

        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.AcceptCard(10 * 1000);
        System.out.println(" [ enable_cardEntryActionPerformed() ] return value: " + returnValue);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ acceptCardActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Operation successfully.", 0);
                break;

            case 1:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 2:
                textLog.insert("\n Channel blocked.", 0);
                break;

            case 3:
                textLog.insert("\n Insertion/return mouth blocked.", 0);
                break;

            case 4:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 5:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end                

    }//GEN-LAST:event_acceptCardActionPerformed

    private void disable_cardEntryActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_disable_cardEntryActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.DisableCardAcceptance(0);
        System.out.println(" [ enable_cardEntryActionPerformed() ] return value: " + returnValue);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ disable_cardEntryActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Operation successfully.", 0);
                break;

            case 1:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 2:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 3:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end



    }//GEN-LAST:event_disable_cardEntryActionPerformed

    private void enable_cardEntryActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_enable_cardEntryActionPerformed

        endingTime = 0;
        totalTime = 0;
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.EnableCardAcceptance(0);
        System.out.println(" [ enable_cardEntryActionPerformed() ] return value: " + returnValue);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for [ enable_cardEntryActionPerformed() ] is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Operation successfully.", 0);
                break;

            case 1:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 2:
                textLog.insert("\n Channel blocked.", 0);
                break;

            case 3:
                textLog.insert("\n Insertion/return mouth blocked.", 0);
                break;

            case 4:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 5:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end

        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for enable_cardEntryActionPerformed is " + totalTime);

    }//GEN-LAST:event_enable_cardEntryActionPerformed

    private void returnCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_returnCardActionPerformed

        cardChannelClearanceMode = 2;
    }//GEN-LAST:event_returnCardActionPerformed

    private void stackCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_stackCardActionPerformed

        cardChannelClearanceMode = 1;
    }//GEN-LAST:event_stackCardActionPerformed

    private void keepCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_keepCardActionPerformed
        //keepCard
        cardChannelClearanceMode = 0;
    }//GEN-LAST:event_keepCardActionPerformed

    private void connectCardActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_connectCardActionPerformed

        endingTime = 0;
        totalTime = 0;
        int port = 0;
        port = serialPortCard.getSelectedIndex();
        int returnValue = 0;
        startingTime = System.currentTimeMillis();
        returnValue = SmartCardObj.ConnectDevice(port, cardChannelClearanceMode, 0);
        System.out.println(" [ connectCardActionPerformed() ] return value: " + returnValue);
        endingTime = System.currentTimeMillis();
        totalTime = (endingTime - startingTime) / 1000;
        System.out.println("Time taken for connectCardActionPerformed is " + totalTime);

        switch (returnValue) {
            case 0:
                textLog.insert("\n Device connected successfully.", 0);
                break;

            case 1:
                textLog.insert("\n Port doesn't exist.", 0);
                break;

            case 2:
                textLog.insert("\n Communication failure.", 0);
                break;

            case 3:
                textLog.insert("\n Channel clearance failed due to rejection bin full.", 0);
                break;

            case 4:
                textLog.insert("\n Channel clearance failed due to return mouth blocked.", 0);
                break;

            case 5:
                textLog.insert("\n Channel clearance failed due to unknown reason.", 0);
                break;

            case 6:
                textLog.insert("\n Operation timeout occurred.", 0);
                break;

            case 7:
                textLog.insert("\n Other error.", 0);
                break;

        }// switch(returnValue) end

    }//GEN-LAST:event_connectCardActionPerformed

    
    private void test_ComportsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_test_ComportsActionPerformed
        
        /*
        ComPortClass ComPortClassObj = new ComPortClass();
        String inputedString = "Hello World";
        String resultString = null;
        char rs='A';
        byte rtS;
        System.out.println(" Result String "+rs);
        int lengthOfString = inputedString.length();
        int ports = (no_of_ports.getSelectedIndex());
        int Com_timeout = 60;
        if("".equals(timeout_ComPortTest.getText()))
        {
            Com_timeout = 60*1000;
        }
        else
        {
            Com_timeout = Integer.valueOf(timeout_ComPortTest.getText())*1000;
        }
        System.out.println("No of Ports : "+ports);
        System.out.println("TimeOut : "+Com_timeout);
        
        for(int counter=0; counter<=ports; counter++)
        {
            System.out.println("Going to Connect Com port : ");
            int conRt = ComPortClassObj.ConnectComPort(counter);
            System.out.println(" Connection Stat "+conRt);
            System.out.println("ComPort "+counter+" Connected");
            
            ComPortClassObj.ClearReceiveBuffer();
            ComPortClassObj.ClearTransmitBuffer();
            
            System.out.println("Buffer Cleared : ");
            
            for(int i = 0; i<lengthOfString; i++)
            {
                System.out.println("writing "+inputedString.charAt(i));
                ComPortClassObj.WriteComPort(inputedString.charAt(i));
                //resultString = resultString+ComPortClassObj.ReadComPort();
                
                rs = ComPortClassObj.ReadComPort();
                rtS = (byte) ComPortClassObj.ReadComPort();
                System.out.println(rtS);
                System.out.println(" Result String "+ComPortClassObj.ReadComPort());
                //System.out.println(" Result String "+resultString);
            }
            
            System.out.println("Complete Result String "+resultString);
            textLog.insert(resultString, 0);
            
            ComPortClassObj.ClearReceiveBuffer();
            ComPortClassObj.ClearTransmitBuffer();
            
            int disConRt = ComPortClassObj.DisConnectComPort();
            System.out.println(" Disconnection Stat "+ disConRt);
            System.out.println("ComPort "+counter+" DisConnected");
            try 
            {
                Thread.sleep(500);
            }
            catch (InterruptedException ex) 
            {
                System.out.println("Thread Sleep Exception : "+ex.getMessage());
            }
            
        }// for(int counter=0; counter<ports; counter++) end
        
        * 
        */
        int ports = (no_of_ports.getSelectedIndex());
        int Com_timeout = 60;
        if("".equals(timeout_ComPortTest.getText()))
        {
            Com_timeout = 10*1000;
        }
        else
        {
            Com_timeout = Integer.valueOf(timeout_ComPortTest.getText())*1000;
        }
        System.out.println("No of Ports : "+(ports+1));
        System.out.println("TimeOut : "+Com_timeout);
        
        /*
        LogFile LogFileObj = new LogFile();
        LogFileObj.createLogFile();
        String SendMessage="HelloWorld";
        System.out.println("Write String Length:"+SendMessage.length() );
        int no_of_times = 0;
        LogFileObj.writeIntoLogFile("No of Ports : "+(ports+1));
        LogFileObj.writeIntoLogFile("TimeOut : "+Com_timeout);
        for(;;)
        {
        //byte[] recv_byte = new byte[ SendMessage.length() ];
            no_of_times++;
            LogFileObj.writeIntoLogFile("Counter : "+no_of_times);
            for(int counter=0; counter<=ports; counter++)
            {
                System.out.println("Port : "+counter);

                LogFileObj.writeIntoLogFile("Port : "+counter+"\n");

                byte[] recv_byte = new byte[ SendMessage.length() ];
                SerialPortComm SerialPortCommObj = new SerialPortComm();
                int conRt = SerialPortCommObj.connectPort("/dev/ttyS" + String.valueOf(counter));
                if(conRt == 1)
                {
                    LogFileObj.writeIntoLogFile("Connection Successfull.");
                }
                else
                {
                    LogFileObj.writeIntoLogFile("Connection Failed.");

                }

                int writeRt = SerialPortCommObj.writePort(SendMessage);
                if(1 == writeRt)
                {
                    LogFileObj.writeIntoLogFile("Write Successfull.\n");
                    LogFileObj.writeIntoLogFile("String Written : "+SendMessage);
                }
                else
                {
                    LogFileObj.writeIntoLogFile("Write Failed.");

                }

                SerialPortCommObj.readPort(recv_byte, SendMessage.length() );
                try 
                {
                    String str = new String(recv_byte, "UTF-8"); // for UTF-8 encoding
                    System.out.println("Receieve String:"+str);
                    LogFileObj.writeIntoLogFile("Receieve String : "+str+"\n");

                } 
                catch (UnsupportedEncodingException ex) 
                {
                    System.out.println("Found UnsupportedEncodingException");
                    LogFileObj.writeIntoLogFile("Found UnsupportedEncodingException");
                }
                finally
                {
                    int disRt = SerialPortCommObj.disconnectPort();
                    if(1==disRt)
                    {
                        LogFileObj.writeIntoLogFile("DisConnection Successfull.\n");
                    }
                    else
                    {
                        LogFileObj.writeIntoLogFile("DisConnection Failed.\n");
                    }
                }// finally end

            }// for(int counter=0; counter<=ports; counter++) end
        }
        */


    }//GEN-LAST:event_test_ComportsActionPerformed
    // private void test_ComportsActionPerformed(java.awt.event.ActionEvent evt) end
    
    
    private void no_of_portsActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_no_of_portsActionPerformed
        // TODO add your handling code here:
    }//GEN-LAST:event_no_of_portsActionPerformed
    
    private void isSAMEnable_buttonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_isSAMEnable_buttonActionPerformed

        int ret = 0;
        String retString = "";
        ret = SmartCardObj.isSAMEnable();
        
        switch(ret)
        {
            case 0: retString = "SAM Not Enabled";
                break;
            
            case 1: retString = "SAM Enabled";
                break;
                
            case 2: retString = "Communication Error";
                break;
            
            case 3: retString = "Timeout Occured";
                break;
            
            case 4: retString = "Reply Byte Not Matched";
                break;
            
            case 5: retString = "Other Error";
                break;
        }// switch(ret) end
        
        textLog.insert("\n SAM Enable Status: "+retString, 0);
    }//GEN-LAST:event_isSAMEnable_buttonActionPerformed

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) {
        /*
         * Set the Nimbus look and feel
         */
        //<editor-fold defaultstate="collapsed" desc=" Look and feel setting code (optional) ">
        /*
         * If Nimbus (introduced in Java SE 6) is not available, stay with the
         * default look and feel. For details see
         * http://download.oracle.com/javase/tutorial/uiswing/lookandfeel/plaf.html
         */
        try {
            for (javax.swing.UIManager.LookAndFeelInfo info : javax.swing.UIManager.getInstalledLookAndFeels()) {
                if ("Nimbus".equals(info.getName())) {
                    javax.swing.UIManager.setLookAndFeel(info.getClassName());
                    break;
                }
            }
        } catch (ClassNotFoundException ex) {
            java.util.logging.Logger.getLogger(Ascrm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (InstantiationException ex) {
            java.util.logging.Logger.getLogger(Ascrm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (IllegalAccessException ex) {
            java.util.logging.Logger.getLogger(Ascrm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        } catch (javax.swing.UnsupportedLookAndFeelException ex) {
            java.util.logging.Logger.getLogger(Ascrm.class.getName()).log(java.util.logging.Level.SEVERE, null, ex);
        }
        //</editor-fold>

        
        /*
         * Create and display the form
         */
        java.awt.EventQueue.invokeLater(new Runnable() {

            public void run() {
                new Ascrm().setVisible(true);
                
                
            }
        });
    }
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel Printer;
    private javax.swing.JButton acceptCard;
    private javax.swing.JButton accept_multinotes;
    private javax.swing.JButton activateSecurityButton;
    private javax.swing.JTextField apduCmd;
    private javax.swing.JTextField apduTime;
    private javax.swing.JButton cardInChannel;
    private javax.swing.ButtonGroup cardManagement_ButtonGroup;
    private javax.swing.JButton cardReaderStatusButton;
    private javax.swing.JButton cardReader_InitButton;
    private javax.swing.JButton cardRemoved;
    private javax.swing.JPanel cardmanagement;
    private javax.swing.JPanel cardreadwrite;
    private javax.swing.JTextField cashBox_duration;
    private javax.swing.JButton clearLog;
    private javax.swing.JButton collectCard_movement;
    private javax.swing.JButton connectCard;
    private javax.swing.JButton contactCard_activate;
    private javax.swing.JButton contactCard_coldreset;
    private javax.swing.JButton contactCard_deactivate;
    private javax.swing.JButton contactCard_poweroff;
    private javax.swing.JButton contactCard_poweron;
    private javax.swing.JButton contactCard_wormreset;
    private javax.swing.JButton contactlessCard_activate;
    private javax.swing.JButton contactlessCard_deactivate;
    private javax.swing.JTextField contactlessCard_page;
    private javax.swing.JButton contactlessCard_read;
    private javax.swing.JButton contactlessCard_write;
    private javax.swing.JButton currencyAccepter_connect;
    private javax.swing.JRadioButton currencyAccepter_keep;
    private javax.swing.JRadioButton currencyAccepter_return;
    private javax.swing.JRadioButton currencyAccepter_stack;
    private javax.swing.ButtonGroup currencyManagement_ButtonGroup;
    private javax.swing.JPanel currencymanagement;
    private javax.swing.JButton deactivateSecurityButton;
    private javax.swing.JButton default_Cancle_Button;
    private javax.swing.JTextField default_Cancle_InputTime;
    private javax.swing.JButton default_Commit_Button;
    private javax.swing.JTextField default_Commit_InputTime;
    private javax.swing.JButton disableAlarm;
    private javax.swing.JButton disable_cardEntry;
    private javax.swing.JButton disconnectCard;
    private javax.swing.JButton disconnectCurrencyAccepter;
    private javax.swing.JButton dispanseCard_movement;
    private javax.swing.JCheckBox displayLog;
    private javax.swing.JButton enable_cardEntry;
    private javax.swing.JButton enable_denoms;
    private javax.swing.JRadioButton fullEjectCard;
    private javax.swing.JButton getBattery_status;
    private javax.swing.JButton getUps_status;
    private javax.swing.JRadioButton halfEjectCard;
    private javax.swing.JRadioButton initCollectCard;
    private javax.swing.JRadioButton initKeepCard;
    private javax.swing.JRadioButton initReturnCard;
    private javax.swing.JButton isCashBoxOpen;
    private javax.swing.JButton isMainDoorOpen;
    private javax.swing.JButton isNoteRemoved_movement;
    private javax.swing.JButton isSAMEnable_button;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel10;
    private javax.swing.JLabel jLabel11;
    private javax.swing.JLabel jLabel12;
    private javax.swing.JLabel jLabel13;
    private javax.swing.JLabel jLabel14;
    private javax.swing.JLabel jLabel15;
    private javax.swing.JLabel jLabel16;
    private javax.swing.JLabel jLabel17;
    private javax.swing.JLabel jLabel18;
    private javax.swing.JLabel jLabel19;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel20;
    private javax.swing.JLabel jLabel21;
    private javax.swing.JLabel jLabel22;
    private javax.swing.JLabel jLabel23;
    private javax.swing.JLabel jLabel24;
    private javax.swing.JLabel jLabel25;
    private javax.swing.JLabel jLabel26;
    private javax.swing.JLabel jLabel27;
    private javax.swing.JLabel jLabel28;
    private javax.swing.JLabel jLabel29;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel30;
    private javax.swing.JLabel jLabel31;
    private javax.swing.JLabel jLabel32;
    private javax.swing.JLabel jLabel33;
    private javax.swing.JLabel jLabel34;
    private javax.swing.JLabel jLabel35;
    private javax.swing.JLabel jLabel36;
    private javax.swing.JLabel jLabel37;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JLabel jLabel6;
    private javax.swing.JLabel jLabel7;
    private javax.swing.JLabel jLabel8;
    private javax.swing.JLabel jLabel9;
    private javax.swing.JLayeredPane jLayeredPane1;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JTextField jTextField3;
    private javax.swing.JRadioButton keepCard;
    private javax.swing.JCheckBox mainDoor_box;
    private javax.swing.JTextField mainDoor_duration;
    private javax.swing.JPanel miscellaneousScreen;
    private javax.swing.JTextField multiAmount_rs;
    private javax.swing.JTextField multiDenom_mask;
    private javax.swing.JLabel multiNoteTimeRem;
    private javax.swing.JComboBox no_of_ports;
    private javax.swing.JButton noteAccepterStatusButton;
    private javax.swing.JButton note_escrow;
    private javax.swing.JButton note_return;
    private javax.swing.JButton note_validate;
    private javax.swing.JRadioButton onPowerSupply;
    private javax.swing.JRadioButton onUpsBackup;
    private javax.swing.JButton playAudioButton;
    private javax.swing.JTabbedPane primtermanagement;
    private javax.swing.JButton printButton;
    private javax.swing.JButton printCups_button;
    private javax.swing.JTextArea printThis;
    private javax.swing.JButton printerOnlineStatus_button;
    private javax.swing.JButton quitButton;
    private javax.swing.JButton read_des;
    private javax.swing.JButton rejectCard_movement;
    private javax.swing.JRadioButton returnCard;
    private javax.swing.JButton returnCard_movement;
    private javax.swing.JTextField returnNoteTime;
    private javax.swing.JButton returnNote_movement;
    private javax.swing.ButtonGroup securityManagement_ButtonGroup;
    private javax.swing.JComboBox securityPort;
    private javax.swing.JPanel securitymanagement;
    private javax.swing.JButton sendT_cl;
    private javax.swing.JButton send_t;
    private javax.swing.JComboBox serialPort;
    private javax.swing.JComboBox serialPortCard;
    private javax.swing.JComboBox serialPortPrinter;
    private javax.swing.JTextField singleDenomTime;
    private javax.swing.JTextField singleNoteDenom;
    private javax.swing.JRadioButton stackCard;
    private javax.swing.JButton stackNote_movement;
    private javax.swing.JTextField stackTime;
    private javax.swing.JButton stopAudioButton;
    private javax.swing.JButton test_Comports;
    private javax.swing.JTextArea textLog;
    private javax.swing.JTextField timeInSec;
    private javax.swing.JTextField timeout_ComPortTest;
    private javax.swing.JLabel validateTime;
    private javax.swing.JButton write_des;
    // End of variables declaration//GEN-END:variables

    public void flagset()
    {
            threadflag = false;
            multiNoteTimeRem.setText("");
            validateTime.setText("");                        
    }// public void flagset() end
    
    
    
    

}

