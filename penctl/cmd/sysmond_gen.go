// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.
/*
 * Package cmd is a auto generated package.
 * Input file: sysmond.proto
 */

package cmd

import (
	"fmt"

	"github.com/spf13/cobra"
)

//cmd - frequency
//rootCmd = system
//longHelpStr = System frequency information:\n\n\nValue Description:\n\nfrequency: Frequency of the system.\nThe frequency is MHz\n\nFrequency	: Frequency of the system in MHz\n
//shortHelpStr = System frequency information
var frequencysystemShowCmd = &cobra.Command{
	Use:   "frequency",
	Short: "System frequency information",
	Long: "\n---------------------------------\n System frequency information:\n\n\nValue Description:\n\nfrequency: Frequency of the system.\nThe frequency is MHz\n\nFrequency	: Frequency of the system in MHz\n\n---------------------------------\n",
	Args: cobra.NoArgs,
	RunE: frequencysystemShowCmdHandler,
}

func frequencysystemShowCmdHandler(cmd *cobra.Command, args []string) error {
	jsonFormat = true
	bodyBytes, err := restGet("telemetry/v1/metrics/asicfrequencymetrics/")
	if err != nil {
		fmt.Println(err)
		return err
	}
	if bodyBytes == nil {
		fmt.Println("No asicfrequencymetrics object(s) found")
	}
	return nil
}

//cmd - memory
//rootCmd = system
//longHelpStr = System memory information:\n\n\nValue Description:\n\nTotal Memory: Total Memory of the system.\nAvailable Memory: Available Memory of the system.\nFree Memory: Free Memory of the system.\nThe memory is KB\n\n
//shortHelpStr = System memory information
var memorysystemShowCmd = &cobra.Command{
	Use:   "memory",
	Short: "System memory information",
	Long:  "\n---------------------------------\n System memory information:\n\n\nValue Description:\n\nTotal Memory: Total Memory of the system.\nAvailable Memory: Available Memory of the system.\nFree Memory: Free Memory of the system.\nThe memory is KB\n\n\n---------------------------------\n",
	Args:  cobra.NoArgs,
	RunE:  memorysystemShowCmdHandler,
}

func memorysystemShowCmdHandler(cmd *cobra.Command, args []string) error {
	jsonFormat = true
	bodyBytes, err := restGet("telemetry/v1/metrics/asicmemorymetrics/")
	if err != nil {
		fmt.Println(err)
		return err
	}
	if bodyBytes == nil {
		fmt.Println("No asicmemorymetrics object(s) found")
	}
	return nil
}

//cmd - power
//rootCmd = system
//longHelpStr = System power information:\n\n\nValue Description:\n\npin: Input power to the system.\npout1: Core output power.\npout2: Arm output power.\nThe power is milli Watt\n\nPin	: Input power to the system\nPout1	: Core output power\nPout2	: ARM output power\n
//shortHelpStr = System power information
var powersystemShowCmd = &cobra.Command{
	Use:   "power",
	Short: "System power information",
	Long: "\n---------------------------------\n System power information:\n\n\nValue Description:\n\npin: Input power to the system.\npout1: Core output power.\npout2: Arm output power.\nThe power is milli Watt\n\nPin	: Input power to the system\nPout1	: Core output power\nPout2	: ARM output power\n\n---------------------------------\n",
	Args: cobra.NoArgs,
	RunE: powersystemShowCmdHandler,
}

func powersystemShowCmdHandler(cmd *cobra.Command, args []string) error {
	jsonFormat = true
	bodyBytes, err := restGet("telemetry/v1/metrics/asicpowermetrics/")
	if err != nil {
		fmt.Println(err)
		return err
	}
	if bodyBytes == nil {
		fmt.Println("No asicpowermetrics object(s) found")
	}
	return nil
}

//cmd - temp
//rootCmd = system
//longHelpStr = System temperature information:\n\n\nValue Description:\n\nlocal_temperature: Temperature of the board.\ndie_temperature: Temperature of the die.\nhbm_temperature: Temperature of the hbm.\nThe temperature is degree Celcius\n\nLocalTemperature	: Temperature of the board in celsius\nDieTemperature	: Temperature of the die in celsius\nHbmTemperature	: Temperature of the HBM in celsius\n
//shortHelpStr = System temperature information
var tempsystemShowCmd = &cobra.Command{
	Use:   "temp",
	Short: "System temperature information",
	Long: "\n---------------------------------\n System temperature information:\n\n\nValue Description:\n\nlocal_temperature: Temperature of the board.\ndie_temperature: Temperature of the die.\nhbm_temperature: Temperature of the hbm.\nThe temperature is degree Celcius\n\nLocalTemperature	: Temperature of the board in celsius\nDieTemperature	: Temperature of the die in celsius\nHbmTemperature	: Temperature of the HBM in celsius\n\n---------------------------------\n",
	Args: cobra.NoArgs,
	RunE: tempsystemShowCmdHandler,
}

func tempsystemShowCmdHandler(cmd *cobra.Command, args []string) error {
	jsonFormat = true
	bodyBytes, err := restGet("telemetry/v1/metrics/asictemperaturemetrics/")
	if err != nil {
		fmt.Println(err)
		return err
	}
	if bodyBytes == nil {
		fmt.Println("No asictemperaturemetrics object(s) found")
	}
	return nil
}

//cmd - system
//rootCmd =
//longHelpStr = Metrics for system monitors
//shortHelpStr = Metrics for system monitors
var systemShowCmd = &cobra.Command{
	Use:   "system",
	Short: "Metrics for system monitors",
	Long:  "\n---------------------------------\n Metrics for system monitors\n---------------------------------\n",
	Args:  cobra.NoArgs,
}

func init() {

	systemShowCmd.AddCommand(frequencysystemShowCmd)

	systemShowCmd.AddCommand(memorysystemShowCmd)

	systemShowCmd.AddCommand(powersystemShowCmd)

	systemShowCmd.AddCommand(tempsystemShowCmd)

	//cmd - system
	//rootCmd =
	//longHelpStr = Metrics for system monitors
	//shortHelpStr = Metrics for system monitors

	metricsShowCmd.AddCommand(systemShowCmd)

}
