/**
 * This file is generated by the SwaggerTSGenerator.
 * Do not edit.
*/
/* tslint:disable */
import { Validators, FormControl, FormGroup, FormArray, ValidatorFn } from '@angular/forms';
import { minValueValidator, maxValueValidator, enumValidator } from './validators';
import { BaseModel, PropInfoItem } from './base-model';

import { Metrics_queryQueryResult, IMetrics_queryQueryResult } from './metrics-query-query-result.model';

export interface IMetrics_queryQueryResponse {
    'tenant'?: string;
    'namespace'?: string;
    'results'?: Array<IMetrics_queryQueryResult>;
}


export class Metrics_queryQueryResponse extends BaseModel implements IMetrics_queryQueryResponse {
    'tenant': string = null;
    'namespace': string = null;
    'results': Array<Metrics_queryQueryResult> = null;
    public static propInfo: { [prop: string]: PropInfoItem } = {
        'tenant': {
            type: 'string'
        },
        'namespace': {
            type: 'string'
        },
        'results': {
            type: 'object'
        },
    }

    public getPropInfo(propName: string): PropInfoItem {
        return Metrics_queryQueryResponse.propInfo[propName];
    }

    /**
     * Returns whether or not there is an enum property with a default value
    */
    public static hasDefaultValue(prop) {
        return (Metrics_queryQueryResponse.propInfo[prop] != null &&
                        Metrics_queryQueryResponse.propInfo[prop].default != null &&
                        Metrics_queryQueryResponse.propInfo[prop].default != '');
    }

    /**
     * constructor
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    constructor(values?: any) {
        super();
        this['results'] = new Array<Metrics_queryQueryResult>();
        this.setValues(values);
    }

    /**
     * set the values for both the Model and the Form Group. If a value isn't provided and we have a default, we use that.
     * @param values Can be used to set a webapi response to this newly constructed model
    */
    setValues(values: any, fillDefaults = true): void {
        if (values && values['tenant'] != null) {
            this['tenant'] = values['tenant'];
        } else if (fillDefaults && Metrics_queryQueryResponse.hasDefaultValue('tenant')) {
            this['tenant'] = Metrics_queryQueryResponse.propInfo['tenant'].default;
        }
        if (values && values['namespace'] != null) {
            this['namespace'] = values['namespace'];
        } else if (fillDefaults && Metrics_queryQueryResponse.hasDefaultValue('namespace')) {
            this['namespace'] = Metrics_queryQueryResponse.propInfo['namespace'].default;
        }
        if (values) {
            this.fillModelArray<Metrics_queryQueryResult>(this, 'results', values['results'], Metrics_queryQueryResult);
        }
        this.setFormGroupValuesToBeModelValues();
    }


    protected getFormGroup(): FormGroup {
        if (!this._formGroup) {
            this._formGroup = new FormGroup({
                'tenant': new FormControl(this['tenant']),
                'namespace': new FormControl(this['namespace']),
                'results': new FormArray([]),
            });
            // generate FormArray control elements
            this.fillFormArray<Metrics_queryQueryResult>('results', this['results'], Metrics_queryQueryResult);
        }
        return this._formGroup;
    }

    setModelToBeFormGroupValues() {
        this.setValues(this.$formGroup.value, false);
    }

    setFormGroupValuesToBeModelValues() {
        if (this._formGroup) {
            this._formGroup.controls['tenant'].setValue(this['tenant']);
            this._formGroup.controls['namespace'].setValue(this['namespace']);
            this.fillModelArray<Metrics_queryQueryResult>(this, 'results', this['results'], Metrics_queryQueryResult);
        }
    }
}

