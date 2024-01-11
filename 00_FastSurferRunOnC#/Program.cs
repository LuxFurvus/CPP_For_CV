using System.Diagnostics;
using System.Text;

internal struct FSData
{
    ///Part you MUST change/////////

    public const string SourceDir = "C:/FastSurferReturn";//Directory where the main folder of the project is created. It must exist and contain SubjectFile before the program starts

    public const string SubjectFile = "Example_Subj.mgz";//The file to be processed. It must be in SourceDir before the program starts

    ///Part you MAY change/////////

    private const string MainDir = "TEST_FOLDER"; //Directory inside SourceDir where the folders FastSurfer needs for work are created.

    private const string FS_InputDir = "FS_InputDir";
    private const string FS_LicenseDir = "FS_LicenseDir";
    private const string FS_OutputDir = "FS_Output_dir";
    private const string SubjectDir = "SubjectDir";

    ///Part you MUST NOT change/////////

    private const string Users = "${userID}:${groupID}";
    public const string LicenceText = "tobox92104@tanlanav.com\n72650\n *CPlk5FrnIe32\n FSMHFeNWjmx/w\n e9A/uPCrFgC0mgDmCyXoAh+eqrrTQg/ZOay4vGEcO7g=";

    public string LicencePath { get; private set; }
    public string SubjectPath { get; private set; }
    public string PowerShellTask { get; private set; }

    public FSData()
    {
        LicencePath = Path.Combine(SourceDir, MainDir, FS_LicenseDir);
        SubjectPath = Path.Combine(SourceDir, MainDir, FS_InputDir, SubjectDir);

        var sb = new StringBuilder();
        sb.Append("docker run ")
          .Append($"-v {SourceDir}/{MainDir}/{FS_InputDir}:/data ")
          .Append($"-v {SourceDir}/{MainDir}/{FS_OutputDir}:/output ")
          .Append($"-v {LicencePath}:/fs_license ")
          .Append($"--rm --user {Users} deepmi/fastsurfer:cpu-v2.2.0 ")
          .Append($"--fs_license /fs_license/license.txt ")
          .Append($"--t1 /data/{SubjectDir}/{SubjectFile} ")
          .Append("--device cpu ")
          .Append($"--sid {SubjectDir} --sd /output --parallel --seg_only ")
          .Append("--allow_root")
          ;
        PowerShellTask = sb.ToString();
    }
}

internal class Program
{
    ////////////////////////////////////////////////////////////
    private static void MakeDir(string path)
    {
        if (!Directory.Exists(path))
        {
            Directory.CreateDirectory(path);
        }
    }

    ////////////////////////////////////////////////////////////
    private static void MakeLicenceFile(FSData FSD)
    {
        MakeDir(FSD.SubjectPath);
        MakeDir(FSD.LicencePath);
        try
        {
            File.WriteAllText(FSD.LicencePath + "/license.txt", FSData.LicenceText);
        }
        catch
        {
            Console.WriteLine("Can't make license.txt\n");
            Environment.Exit(0);
        }
    }

    ////////////////////////////////////////////////////////////
    private static void MoveSubjectFile(FSData FSD)
    {
        string SourceFilePath = Path.Combine(FSData.SourceDir,
            FSData.SubjectFile);
        string SubjectFilePath = Path.Combine(FSD.SubjectPath,
            FSData.SubjectFile);

        if (File.Exists(SourceFilePath))
        {
            try
            {
                File.Move(SourceFilePath, SubjectFilePath);
                return;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error moving file: {ex.Message}");
                Environment.Exit(0);
            }
        }
    }

    ////////////////////////////////////////////////////////////
    private static bool RunPowerShellTask(string task)
    {
        // Create a new process to run PowerShell
        Process process = new Process
        {
            StartInfo = new ProcessStartInfo
            {
                FileName = "powershell.exe",
                RedirectStandardInput = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                CreateNoWindow = true,
                UseShellExecute = false,
                WorkingDirectory = Environment.CurrentDirectory,
                Verb = "runas" // Run PowerShell with administrator privileges if needed
            }
        };

        // Event handler to capture real-time output
        process.OutputDataReceived += (sender, e) => Console.WriteLine($"{e.Data}");
        process.ErrorDataReceived += (sender, e) =>
        {
            if (!string.IsNullOrWhiteSpace(e.Data))
            {
                Console.WriteLine($"--| {e.Data}");
            }
        };

        // Start the process
        process.Start();

        // Begin redirecting standard output and error
        process.BeginOutputReadLine();
        process.BeginErrorReadLine();

        // Send the PowerShell command to the standard input
        process.StandardInput.WriteLine(task);

        // Send exit command to close PowerShell after executing the command
        process.StandardInput.WriteLine("exit");

        // Wait for the process to exit
        process.WaitForExit();

        // Close the standard input stream
        process.StandardInput.Close();

        // Close the process
        process.Close();

        return true;
    }

    ////////////////////////////////////////////////////////////
    private static void Main()
    {
        FSData FSD = new();
        MakeLicenceFile(FSD);
        MoveSubjectFile(FSD);

        Console.OutputEncoding = System.Text.Encoding.UTF8;//This part provides (partially) correct visualisation of output

        //string TestTask = "1..10 | ForEach-Object { Start-Sleep -Seconds 1; Write-Host $_ }";//Dummy PowerShell task to test RunPowerShellTask() function

        if (!RunPowerShellTask(FSD.PowerShellTask))
        {//This part is useless. RunPowerShellTask() always returns true
            Console.WriteLine("\nTASK failed!");
            Environment.Exit(0);
        }

        Console.WriteLine("\nTASK completed successfully!");
    }
}