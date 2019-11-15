/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */
import { Validators, FormControl, FormGroup, FormArray, ValidatorFn } from '@angular/forms';
import { minValueValidator, maxValueValidator, minLengthValidator, maxLengthValidator, required, enumValidator, patternValidator, CustomFormControl, CustomFormGroup } from '../../../utils/validators';
import { BaseModel, PropInfoItem } from '../basemodel/base-model';

import { ConfigurationSnapshotStatusConfigSaveStatus_dest_type,  } from './enums';

export interface IConfigurationSnapshotStatusConfigSaveStatus {
    'dest-type': ConfigurationSnapshotStatusConfigSaveStatus_dest_type;
    'uri'?: string;
}


export class ConfigurationSnapshotStatusConfigSaveStatus extends BaseModel implements IConfigurationSnapshotStatusConfigSaveStatus {
    'dest-type': ConfigurationSnapshotStatusConfigSaveStatus_dest_type = null;
    'uri': string = null;
    public static propInfo: { [prop in keyof IConfigurationSnapshotStatusConfigSaveStatus]: PropInfoItem } = {
        'dest-type': {
            enum: ConfigurationSnapshotStatusConfigSaveStatus_dest_type,
            default: 'objectstore',
            required: true,
            type: 'string'
        },
        'uri': {
            required: false,
            type: 'string'
        },
    }

    public getPropInfo(propName: string): PropInfoItem {
        return ConfigurationSnapshotStatusConfigSaveStatus.propInfo[propName];
    }

    public getPropInfoConfig(): { [key:string]:PropInfoItem } {
        return ConfigurationSnapshotStatusConfigSaveStatus.propInfo;
    }

    /**
     * Returns whether or not there is an enum property with a default value
    */
    public static hasDefaultValue(prop) {
        return (ConfigurationSnapshotStatusConfigSaveStatus.propInfo[prop] != null &&
                        ConfigurationSnapshotStatusConfigSaveStatus.propInfo[prop].default != null);
    }

    /**
     * constructor
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    constructor(values?: any, setDefaults:boolean = true) {
        super();
        this._inputValue = values;
        this.setValues(values, setDefaults);
    }

    /**
     * set the values for both the Model and the Form Group. If a value isn't provided and we have a default, we use that.
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    setValues(values: any, fillDefaults = true): void {
        if (values && values['dest-type'] != null) {
            this['dest-type'] = values['dest-type'];
        } else if (fillDefaults && ConfigurationSnapshotStatusConfigSaveStatus.hasDefaultValue('dest-type')) {
            this['dest-type'] = <ConfigurationSnapshotStatusConfigSaveStatus_dest_type>  ConfigurationSnapshotStatusConfigSaveStatus.propInfo['dest-type'].default;
        } else {
            this['dest-type'] = null
        }
        if (values && values['uri'] != null) {
            this['uri'] = values['uri'];
        } else if (fillDefaults && ConfigurationSnapshotStatusConfigSaveStatus.hasDefaultValue('uri')) {
            this['uri'] = ConfigurationSnapshotStatusConfigSaveStatus.propInfo['uri'].default;
        } else {
            this['uri'] = null
        }
        this.setFormGroupValuesToBeModelValues();
    }


    protected getFormGroup(): FormGroup {
        if (!this._formGroup) {
            this._formGroup = new FormGroup({
                'dest-type': CustomFormControl(new FormControl(this['dest-type'], [required, enumValidator(ConfigurationSnapshotStatusConfigSaveStatus_dest_type), ]), ConfigurationSnapshotStatusConfigSaveStatus.propInfo['dest-type']),
                'uri': CustomFormControl(new FormControl(this['uri']), ConfigurationSnapshotStatusConfigSaveStatus.propInfo['uri']),
            });
        }
        return this._formGroup;
    }

    setModelToBeFormGroupValues() {
        this.setValues(this.$formGroup.value, false);
    }

    setFormGroupValuesToBeModelValues() {
        if (this._formGroup) {
            this._formGroup.controls['dest-type'].setValue(this['dest-type']);
            this._formGroup.controls['uri'].setValue(this['uri']);
        }
    }
}

