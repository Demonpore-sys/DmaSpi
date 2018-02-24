#ifndef CHIPSELECT_H
#define CHIPSELECT_H

#include <core_pins.h>

/** \brief Specifies the desired CS suppression
**/
enum TransferType
{
  NORMAL,      //*< The transfer will use CS at beginning and end **/
  NO_START_CS, //*< Skip the CS activation at the start **/
  NO_END_CS    //*< SKip the CS deactivation at the end **/
};

/** \brief An abstract base class that provides an interface for chip select classes.
**/
class AbstractChipSelect
{
	public:
    /** \brief Called to select a chip. The implementing class can do other things as well.
    **/
    virtual void select(TransferType transferType = TransferType::NORMAL) = 0;

    /** \brief Called to deselect a chip. The implementing class can do other things as well.
    **/
    virtual void deselect(TransferType transferType = TransferType::NORMAL) = 0;

    /** \brief the virtual destructor needed to inherit from this class **/
		virtual ~AbstractChipSelect() {}
};


/** \brief "do nothing" chip select class **/
class DummyChipSelect : public AbstractChipSelect
{
  void select(TransferType transferType = TransferType::NORMAL) override {}

  void deselect(TransferType transferType = TransferType::NORMAL) override {}
};

/** \brief "do nothing" chip select class that
 * outputs a message through Serial when something happens
**/
class DebugChipSelect : public AbstractChipSelect
{
  void select(TransferType transferType = TransferType::NORMAL) override {Serial.println("Debug CS: select()");}
  void deselect(TransferType transferType = TransferType::NORMAL) override {Serial.println("Debug CS: deselect()");}
};

/** \brief An active low chip select class. This also configures the given pin.
 * Warning: This class is hardcoded to manage a transaction on SPI (SPI0, that is).
 * If you want to use SPI1: Use AbstractChipSelect1 (see below)
 * If you want to use SPI2: Create AbstractChipSelect2 (adapt the implementation accordingly).
 * Something more flexible is on the way.
**/
class ActiveLowChipSelect : public AbstractChipSelect
{
  public:
    /** Configures a chip select pin for OUTPUT mode,
     * manages the chip selection and a corresponding SPI transaction
     *
     * The chip select pin is asserted \e after the SPI settings are applied
     * and deasserted before the SPI transaction ends.
     * \param pin the CS pin to use
     * \param settings which SPI settings to apply when the chip is selected
    **/
    ActiveLowChipSelect(const unsigned int& pin, const SPISettings& settings)
      : pin_(pin),
      settings_(settings)
    {
      pinMode(pin, OUTPUT);
      digitalWriteFast(pin, 1);
    }

    /** \brief begins an SPI transaction selects the chip (sets the pin to low) and
    **/
    void select(TransferType transferType = TransferType::NORMAL) override
    {
      SPI.beginTransaction(settings_);
      if (transferType == TransferType::NO_START_CS) {
    	  return;
      }
      digitalWriteFast(pin_, 0);
    }

    /** \brief deselects the chip (sets the pin to high) and ends the SPI transaction
    **/
    void deselect(TransferType transferType = TransferType::NORMAL) override
    {
      if (transferType == TransferType::NO_END_CS) {
      } else {
    	  digitalWriteFast(pin_, 1);
      }
      SPI.endTransaction();
    }
  private:
    const unsigned int pin_;
    const SPISettings settings_;

};

#if defined(__MK66FX1M0__)
class ActiveLowChipSelect1 : public AbstractChipSelect
{
  public:
    /** Equivalent to AbstractChipSelect, but for SPI1.
    **/
    ActiveLowChipSelect1(const unsigned int& pin, const SPISettings& settings)
      : pin_(pin),
      settings_(settings)
    {
      pinMode(pin, OUTPUT);
      digitalWriteFast(pin, 1);
    }

    /** \brief begins an SPI transaction selects the chip (sets the pin to low) and
    **/
    void select(TransferType transferType = TransferType::NORMAL) override
    {
      SPI1.beginTransaction(settings_);
      if (transferType == TransferType::NO_START_CS) {
        return;
      }
      digitalWriteFast(pin_, 0);
    }

    /** \brief deselects the chip (sets the pin to high) and ends the SPI transaction
    **/
    void deselect(TransferType transferType = TransferType::NORMAL) override
    {
      if (transferType == TransferType::NO_END_CS) {
      } else {
        digitalWriteFast(pin_, 1);
      }
      SPI1.endTransaction();
    }
  private:
    const unsigned int pin_;
    const SPISettings settings_;

};
#endif


#endif // CHIPSELECT_H

